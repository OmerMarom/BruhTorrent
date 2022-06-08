
#include "base/bt_pch.h"
#include "tcp_service.h"

#include <utility>

using namespace std::placeholders;

namespace bt {
	tcp_service::tcp_service(bt::endpoint ep, boost::asio::io_context& io_ctx,
							 on_connected callback) :
		m_socket(io_ctx),
		m_endpoint(std::move(ep)) {
		connect(std::move(callback));
	}

	tcp_service::tcp_service(boost::asio::ip::tcp::socket socket) :
		m_socket(std::move(socket)),
		m_endpoint(m_socket.remote_endpoint().address().to_string(),
				   m_socket.remote_endpoint().port())
	{ }

	tcp_service::tcp_service(tcp_service&& other) noexcept :
		m_socket(std::move(other.m_socket)),
		m_endpoint(std::move(other.m_endpoint))
	{ }

	void tcp_service::connect(on_connected callback) {
		const auto& [ip, port] = m_endpoint;
		const boost::asio::ip::tcp::endpoint boost_ep(boost::asio::ip::address::from_string(ip), port);
		m_socket.async_connect(boost_ep, [this, callback = std::move(callback)](const auto& err) {
			callback(err ? error(connection_error, err.to_string()) : error());
		});
	}

	void tcp_service::send(buffer send_buf, on_sent callback) {
		const auto shared_send_buf = std::make_shared<buffer>(std::move(send_buf));
		boost::asio::async_write(
			m_socket, boost::asio::buffer(*shared_send_buf),
			[this, shared_send_buf, callback = std::move(callback)]
			(const auto& err, const auto bytes_transferred) {
				error bt_err;
				if (err) {
					bt_err = { send_error, err.to_string() };
				} else if (bytes_transferred != shared_send_buf->size()) {
					bt_err = { send_error, "TCP send size doesn't match send buffer size." };
				}
				callback(bt_err);
			}
		);
	}

	void tcp_service::receive(on_received callback) {
        receive_impl(std::make_shared<buffer>(), std::move(callback));
	}

    void tcp_service::receive_impl(std::shared_ptr<buffer> shared_receive_buf, on_received callback) {
        // TODO: Impl - add interval to receives.
        receive_once_impl(
            shared_receive_buf,
            [this, srb = std::move(shared_receive_buf), callback = std::move(callback)]
            (auto&& r_receive_buf) mutable {
                callback(std::forward<decltype(r_receive_buf)>(r_receive_buf));
                receive_impl(std::move(srb), std::move(callback));
            }
        );
    }

    void tcp_service::receive_once(on_received callback) {
        receive_once_impl(std::make_shared<buffer>(), std::move(callback));
    }

    void tcp_service::receive_once_impl(std::shared_ptr<buffer> shared_receive_buf, on_received callback) {
        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(*shared_receive_buf),
            [srb = std::move(shared_receive_buf), callback = std::move(callback)]
            (const auto& err, const auto bytes_transferred) {
                if (err) {
                    callback(error(receive_error, err.to_string()));
                } else {
                    callback(const_buffer_ref(srb->begin(), bytes_transferred));
                }
            }
        );
    }
}
