#pragma once

#include "alert_service.h"
#include "peer.h"
#include "peer_messages.h"

#define STD_PH std::placeholders

namespace bt {
	class peer_messenger {
	public:
		peer_messenger(endpoint ep, peer& p, boost::asio::io_context& io_ctx, alert_service& alert_service);

		void send_message(const peer_messages::peer_message& msg);

		static constexpr error_code_t connection_error = 1;
		static constexpr error_code_t send_error = 2;

	private:
		void connect(endpoint ep);

		void on_message_sent(const boost::system::error_code& error,
							 std::size_t bytes_transferred,
							 const char* msg_type,
							 std::size_t msg_size) const;

		peer& m_peer;
		boost::asio::ip::tcp::socket m_socket;
		alert_service& m_alert_service;
	};
}
