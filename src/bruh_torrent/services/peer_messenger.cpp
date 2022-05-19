
#include "base/bt_pch.h"
#include "peer_messenger.h"
#include "utils/endpoint.h"

namespace bt {
	// TODO: Impl - Use strand to sync access to socket.

	peer_messenger::peer_messenger(endpoint ep, peer& p, boost::asio::io_context& io_ctx, alert_service& alert_service) :
		m_peer(p),
		m_socket(io_ctx),
		m_alert_service(alert_service) {
		connect(std::move(ep));
	}

	void peer_messenger::connect(const endpoint ep) {
		try {
			m_socket.connect({ boost::asio::ip::address::from_string(ep.ip), ep.port });
		} catch (const boost::system::system_error& se) {
			m_alert_service.notify_error({ connection_error, se.what() });
			// TODO: Impl - What to do in case of an error?
		}
	}

	void peer_messenger::send_message(const peer_messages::peer_message& msg) {
		auto msg_buffer = msg.to_buffer();
		const char* msg_type = msg.type();
		boost::asio::async_write(
			m_socket, boost::asio::buffer(msg_buffer),
			[this, m = std::move(msg_buffer), msg_type](const auto& error, const auto bytes_transferred) mutable {
				on_message_sent(error, bytes_transferred, msg_type, m.size());
			}
		);
	}

	void peer_messenger::on_message_sent(const boost::system::error_code& error,
										 const std::size_t bytes_transferred,
										 const char* msg_type,
										 const std::size_t msg_size) const
	{
		if (error) {
			m_alert_service.notify_error({ send_error, error.to_string() });
			// TODO: Impl - What to do in case of an error?
		} else {
			BT_ASSERT(
				bytes_transferred == msg_size,
				"Peer message send size doesn't match message size & no socket error was reported."
			);
			m_alert_service.notify_info(fmt::format(
				"Message {} sent to peer {} successfully.",
				msg_type, m_peer.id()
			));
		}
	}
}
