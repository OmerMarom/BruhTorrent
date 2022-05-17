#pragma once

#include "alert_service.h"
#include "peer.h"
#include "utils/types.h"

#define STD_PH std::placeholders

namespace bt {
	namespace peer_messages {
		struct has_piece_msg {
			piece_idx_t piece_idx;
			inline static const char* type = "HAS_PIECE";
		};

		struct request_piece_msg {
			piece_idx_t piece_idx;
			inline static const char* type = "REQUEST_PIECE";
		};
	}

	class peer_messenger {
	public:
		peer_messenger(endpoint ep, peer& p, boost::asio::io_context& io_ctx, alert_service& alert_service);

		void send_request_piece(peer_messages::request_piece_msg msg);

		void send_has_piece(peer_messages::has_piece_msg msg);

		static constexpr error_code_t connection_error = 1;
		static constexpr error_code_t send_error = 2;

	private:
		void connect(endpoint ep);

		template <typename Tmsg>
		void send_message(Tmsg msg);

		template <typename Tmsg>
		void send_message_impl(std::unique_ptr<Tmsg> msg, std::size_t total_bytes_transferred = 0);

		template <typename Tmsg>
		void on_message_sent(std::unique_ptr<Tmsg> msg, const boost::system::error_code& error,
							 std::size_t total_bytes_transferred, std::size_t bytes_transferred);

		peer& m_peer;
		boost::asio::ip::tcp::socket m_socket;
		alert_service& m_alert_service;
	};

	// Template methods impl:

	template <typename Tmsg>
	void peer_messenger::send_message(Tmsg msg) {
		// We are only passing a ref to async_send so we need to make sure msg's lifetime is longer then the send operation.
		// We achieve this by storing its value in a unique_ptr & binding it to the handler, which is guaranteed to outlive the send. 
		auto msg_holder = std::make_unique<Tmsg>(std::move(msg));
		send_message_impl(std::move(msg_holder));
	}

	template <typename Tmsg>
	void peer_messenger::send_message_impl(std::unique_ptr<Tmsg> msg, const std::size_t total_bytes_transferred) {
		m_socket.async_send(
			boost::asio::buffer((void*)msg.get(), sizeof(Tmsg)),
			[this, m = std::move(msg), total_bytes_transferred](const auto& error, const auto bytes_transferred) mutable {
				on_message_sent(std::move(m), error, total_bytes_transferred, bytes_transferred);
			}
		);
	}

	template <typename Tmsg>
	void peer_messenger::on_message_sent(std::unique_ptr<Tmsg> msg, const boost::system::error_code& error,
										 std::size_t total_bytes_transferred, const std::size_t bytes_transferred) {
		if (error) {
			m_alert_service.notify_error({ send_error, error.to_string() });
			// TODO: Impl - What to do in case of an error?
		}
		else if (total_bytes_transferred += bytes_transferred; total_bytes_transferred < sizeof(Tmsg)) {
			send_message_impl(std::move(msg), total_bytes_transferred);
		}
		else {
			m_alert_service.notify_info(fmt::format("Message {} sent to peer {} successfully.", Tmsg::type, m_peer.id()));
		}
	}
}
