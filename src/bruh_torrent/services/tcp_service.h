#pragma once

#include "base/result.h"
#include "base/types.h"
#include "utils/endpoint.h"

namespace bt {
	class tcp_service {
	public:
		using on_connected = std::function<void(const error&)>;
		using on_sent = std::function<void(const error&)>;
		using on_received = std::function<void(result<const_buffer_ref>)>;

		// Ctor for connection that we need to initiate:
		tcp_service(endpoint ep, boost::asio::io_context& io_ctx, on_connected callback);

		// Ctor for connection that was initiated by remote:
		tcp_service(boost::asio::ip::tcp::socket socket);

		tcp_service(tcp_service&& other) noexcept;

		[[nodiscard]] const endpoint& endpoint() const { return m_endpoint; }

		void send(buffer send_buf, on_sent callback);

		void receive(on_received callback);

		static constexpr error_code_t connection_error = 2;
		static constexpr error_code_t send_error = 3;
		static constexpr error_code_t receive_error = 4;

	private:
		void connect(on_connected callback);

		static constexpr std::size_t msg_buffer_size = 10000;

		boost::asio::ip::tcp::socket m_socket;
		bt::endpoint m_endpoint;
		buffer m_msg_buf = buffer(msg_buffer_size);
	};
}
