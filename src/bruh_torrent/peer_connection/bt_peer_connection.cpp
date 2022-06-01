
#include "base/bt_pch.h"
#include "bt_peer_connection.h"

#include "bruh_torrent.h"
#include "torrent.h"
#include "peer_messages.h"
#include "services/alert_service.h"

using namespace std::placeholders;

namespace bt {
	bt_peer_connection::bt_peer_connection(torrent& tor, alert_service& as,
										   endpoint ep, boost::asio::io_context& io_ctx) :
		peer_connection(&tor, as),
		m_tcp(std::move(ep), io_ctx,
			  std::bind(&bt_peer_connection::on_connected, this, _1))
	{ }

	bt_peer_connection::bt_peer_connection(alert_service& as, tcp_service tcp) :
		peer_connection(nullptr, as),
		m_tcp(std::move(tcp)) {
		m_tcp.receive(
			std::bind(&bt_peer_connection::on_init_conn_received, this, _1));
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
		}
	}

	void bt_peer_connection::request_piece(const piece_idx_t piece_idx) {
		send_message(peer_messages::request_piece_msg(piece_idx));
	}

	void bt_peer_connection::update_has_piece(const piece_idx_t piece_idx) {
		send_message(peer_messages::has_piece_msg(piece_idx));
	}

	void bt_peer_connection::on_has_piece(const peer_messages::has_piece_msg& hp_msg) {
		if (!m_pieces_in_possession) {
			// TODO: Impl - Handle bitfield wasn't initialized.
		} else if (hp_msg.piece_idx > m_pieces_in_possession->size()) {
			m_alert_service.notify_error({
				invalid_piece_idx,
				fmt::format("Received invalid piece idx from peer at {}.", m_tcp.endpoint().ip)
			});
			// TODO: Impl - Handle invalid message.
		} else {
			m_pieces_in_possession->at(hp_msg.piece_idx) = true;
		}
	}

	void bt_peer_connection::on_req_piece(const peer_messages::request_piece_msg& rp_msg) const {
		if (!m_pieces_in_possession) {
			// TODO: Impl - Handle bitfield wasn't initialized.
		} else if (rp_msg.piece_idx > m_pieces_in_possession->size()) {
			m_alert_service.notify_error({
				invalid_piece_idx,
				fmt::format("Received invalid piece idx from peer at {}.", m_tcp.endpoint().ip)
				});
			// TODO: Impl - Handle invalid message.
		} else if (!m_pieces_in_possession->at(rp_msg.piece_idx)) {
			// TODO: Impl - Handle we don't have piece.
		} else {
			// TODO: Impl - Send piece.
		}
	}

	void bt_peer_connection::on_conn_res_received(result<const_buffer_ref> r_msg_buf) {
		if (!r_msg_buf) {
			m_alert_service.notify_error(r_msg_buf.error());
			// TODO: Impl - Handle error.
		} else {
			auto r_cr_msg = peer_messages::conn_res_msg::from_buffer(*r_msg_buf);
			if (!r_cr_msg) {
				m_alert_service.notify_error(r_cr_msg.error());
				// TODO: Impl - Handle msg invalid.
			} else if (!r_cr_msg->has_tor) {
				// TODO: Impl - Handle peer doesn't have torrent.
			} else {
				m_pieces_in_possession = std::move(r_cr_msg->pieces_in_possession);
				m_tcp.receive(std::bind(&bt_peer_connection::on_msg_received, this, _1));
			}
		}
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
					// TODO: Impl - Handle no torrent (send false in has_tor field of conn_res).
				} else {
					m_tcp.receive(std::bind(&bt_peer_connection::on_msg_received, this, _1));
					// TODO: Impl - Send conn_res_msg.
				}
			}
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
		// TODO: Impl - Maybe make receive_once and receive in tcp_service.
	}
}

