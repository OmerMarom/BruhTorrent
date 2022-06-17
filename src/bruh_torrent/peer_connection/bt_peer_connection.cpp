
#include "base/bt_pch.h"
#include "bt_peer_connection.h"

#include "bruh_torrent.h"
#include "torrent.h"
#include "peer_messages.h"
#include "services/alert_service.h"

using namespace std::placeholders;

namespace bt {
	bt_peer_connection::bt_peer_connection(torrent& tor, bruh_torrent& bt, alert_service& as,
										   endpoint ep, boost::asio::io_context& io_ctx) :
		peer_connection(&tor, bt, as),
		m_tcp(std::move(ep), io_ctx, BIND_THIS_SINGLE_ARG(on_connected))
	{ }

	bt_peer_connection::bt_peer_connection(alert_service& as, bruh_torrent& bt, tcp_service tcp) :
		peer_connection(nullptr, bt, as),
		m_tcp(std::move(tcp)) {
		m_tcp.receive_once(BIND_THIS_SINGLE_ARG(on_init_conn_received));
	}

	void bt_peer_connection::on_connected(const error& err) {
		if (err) {
			m_alert_service.notify_error(err);
			// TODO: Impl - Handle connection fails.
		} else {
			m_alert_service.notify_info(fmt::format(
				"TCP connection established to peer at {}.", m_tcp.endpoint().ip
			));
			send_message(peer_messages::init_conn_msg(m_torrent->id()));
            m_tcp.receive_once(BIND_THIS_SINGLE_ARG(on_conn_res_received));
		}
	}

	void bt_peer_connection::request_piece(const piece_idx_t piece_idx) {
		send_message(peer_messages::request_piece_msg(piece_idx));
	}

	void bt_peer_connection::update_has_piece(const piece_idx_t piece_idx) {
		send_message(peer_messages::has_piece_msg(piece_idx));
	}

	void bt_peer_connection::on_has_piece(const peer_messages::has_piece_msg& hp_msg) {
        if (hp_msg.piece_idx >= m_torrent->num_of_pieces()) {
            // TODO: Impl - Handle invalid piece_idx.
        } else {
			m_pieces_in_possession->at(hp_msg.piece_idx) = true;
		}
	}

	void bt_peer_connection::on_req_piece(const peer_messages::request_piece_msg& rp_msg) {
        const auto piece_idx = rp_msg.piece_idx;
        m_torrent->get_piece(piece_idx, [this, piece_idx](auto r_piece_buf) {
            if (!r_piece_buf) {
                // TODO: Impl - Handle can't get piece.
            } else {
                send_message(peer_messages::piece_msg(piece_idx, std::move(*r_piece_buf)));
            }
        });
	}

    void bt_peer_connection::on_piece(peer_messages::piece_msg p_msg) {
        m_torrent->on_piece(
            p_msg.piece_idx, std::move(p_msg.piece_buf),
            [](error&& err) {
                if (err) {
                    // TODO: Impl - handle error.
                }
            }
        );
    }

	void bt_peer_connection::on_init_conn_received(result<const_buffer_ref> r_msg_buf) {
		if (!r_msg_buf) {
			m_alert_service.notify_error(r_msg_buf.error());
			// TODO: Impl - Handle error.
		} else {
			auto r_ic_msg = peer_messages::init_conn_msg::from_buffer(*r_msg_buf);
			if (!r_ic_msg) {
				m_alert_service.notify_error(r_ic_msg.error());
				// TODO: Impl - Handle msg invalid.
			} else {
				m_torrent = m_bt.get_torrent(r_ic_msg->tor_id);
				if (!m_torrent) {
                    send_message(peer_messages::conn_res_msg());
                    // TODO: Impl - Handle no torrent.
				} else {
					m_tcp.receive(BIND_THIS_SINGLE_ARG(on_msg_received));
                    send_message(peer_messages::conn_res_msg(m_torrent->pieces_in_possession()));
				}
			}
		}
	}

    void bt_peer_connection::on_conn_res_received(result<const_buffer_ref> r_msg_buf) {
        if (!r_msg_buf) {
            m_alert_service.notify_error(r_msg_buf.error());
            // TODO: Impl - Handle error.
            return;
        }
        auto r_cr_msg = peer_messages::conn_res_msg::from_buffer(*r_msg_buf);
        if (!r_cr_msg) {
            m_alert_service.notify_error(r_cr_msg.error());
            // TODO: Impl - Handle msg invalid.
        } else if (!r_cr_msg->has_tor()) {
            // TODO: Impl - Handle peer doesn't have torrent.
        } else if (r_cr_msg->pieces_in_possession.size() != m_torrent->num_of_pieces()) {
            m_alert_service.notify_error({
                invalid_num_of_pieces,
                fmt::format(
                    "Received a CONN_RES message with an invalid no. of pieces from peer at {}.",
                    m_tcp.endpoint().ip
                )
            });
        } else {
            m_pieces_in_possession = std::move(r_cr_msg->pieces_in_possession);
            m_tcp.receive(BIND_THIS_SINGLE_ARG(on_msg_received));
            // TODO: Impl - Should I start requesting pieces here?
        }
    }

	void bt_peer_connection::send_message(const peer_messages::peer_message& msg) {
		m_tcp.send(msg.to_buffer(), [this, msg_name = msg.name()](const error& err) {
			if (err) {
				m_alert_service.notify_error(err);
				// TODO: Impl - Handle error.
			} else {
				m_alert_service.notify_info(fmt::format(
					"Message {} sent to peer at {} successfully.",
					msg_name, m_tcp.endpoint().ip
				));
			}
		});
	}

	void bt_peer_connection::on_msg_received(const result<const_buffer_ref> r_msg_buf) {
		if (!r_msg_buf) {
			m_alert_service.notify_error(r_msg_buf.error());
			// TODO: Impl - Handle error.
		} else {
			peer_messages::from_buffer(*r_msg_buf, *this);
		}
	}
}


