#pragma once

#include "peer_connection.h"
#include "services/tcp_service.h"

namespace bt {
	namespace peer_messages {
		struct peer_message;
		struct request_piece_msg;
		struct has_piece_msg;
	}

	class BT_API bt_peer_connection : public peer_connection {
	public:
		bt_peer_connection(torrent& tor, bruh_torrent& bt, alert_service& as,
						   endpoint ep, boost::asio::io_context& io_ctx);

		bt_peer_connection(alert_service& as, bruh_torrent& bt, tcp_service tcp);

		// Outgoing messages:
		void request_piece(piece_idx_t piece_idx) override;
		void update_has_piece(piece_idx_t piece_idx) override;

		// Incoming messages:
		void on_has_piece(const peer_messages::has_piece_msg& hp_msg);
		void on_req_piece(const peer_messages::request_piece_msg& rp_msg) const;

		static constexpr error_code_t invalid_piece_idx = 2;

	private:
		void on_conn_res_received(result<const_buffer_ref> r_msg_buf);
		void on_init_conn_received(result<const_buffer_ref> r_msg_buf);

		void on_connected(const error& err);
		void on_msg_received(result<const_buffer_ref> r_msg_buf);
		void send_message(const peer_messages::peer_message& msg);

		tcp_service m_tcp;
	};
}