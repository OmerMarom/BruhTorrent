
#include "bt_pch.h"
#include "peer_messenger.h"
#include "utils/endpoint.h"

namespace bt {
	peer_messenger::peer_messenger(endpoint ep, peer& p, boost::asio::io_context& io_ctx, alert_service& alert_service) :
		m_peer(p),
		m_socket(io_ctx),
		m_alert_service(alert_service) {
		connect(std::move(ep));
	}

	void peer_messenger::connect(const endpoint ep) {
		try {
			m_socket.connect({ boost::asio::ip::address::from_string(ep.ip), ep.port });
		}
		catch (const boost::system::system_error& se) {
			m_alert_service.notify_error({ connection_error, se.what() });
		}
	}

	void peer_messenger::send_request_piece(const peer_messages::request_piece_msg msg) {
		send_message(msg);
	}

	void peer_messenger::send_has_piece(const peer_messages::has_piece_msg msg) {
		send_message(msg);
	}
}
