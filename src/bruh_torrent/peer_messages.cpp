
#include "peer_messages.h"
#include "peer_connection/bt_peer_connection.h"

namespace bt::peer_messages {
	error from_buffer(const const_buffer_ref& msg_buf, bt_peer_connection& bt_pc) {
		auto* buf_ptr = msg_buf.data();
		const const_buffer_ref msg_buf_ref(
			msg_buf.begin() + sizeof(msg_id_t), msg_buf.end());
		switch (const auto msg_id = read_from_buffer<msg_id_t>(buf_ptr)) {
			case has_piece_msg::msg_id: {
				auto r_has_piece = has_piece_msg::from_buffer(msg_buf_ref);
				if (!r_has_piece) return r_has_piece.error();
				bt_pc.on_has_piece(*r_has_piece);
				break;
			} case request_piece_msg::msg_id: {
				auto r_req_piece = request_piece_msg::from_buffer(msg_buf_ref);
				if (!r_req_piece) return r_req_piece.error();
				bt_pc.on_req_piece(*r_req_piece);
				break;
			} case piece_msg::msg_id: {
                auto r_piece = piece_msg::from_buffer(msg_buf_ref);
                if (!r_piece) return r_piece.error();
                bt_pc.on_piece(std::move(*r_piece));
                break;
            } default: {
				return {
					invalid_msg_id,
					fmt::format("Received message with invalid ID {}.", msg_id)
				};
			}
		}
		return {};
	}

	buffer peer_message::to_buffer() const {
		buffer msg_buf(sizeof(msg_id_t) + msg_size());
		auto* buf_ptr = msg_buf.data();
		write_to_buffer(id(), buf_ptr);
		buffer_ref msg_buf_ref(msg_buf.begin(), msg_size());
		to_buffer_impl(msg_buf_ref);
		return msg_buf;
	}

	void init_conn_msg::to_buffer_impl(buffer_ref& msg_buf) const {
		auto* buf_ptr = msg_buf.data();
		write_to_buffer(tor_id, buf_ptr);
	}

    result<init_conn_msg> init_conn_msg::from_buffer(const const_buffer_ref &msg_buf) {
        if (msg_buf.size() != init_conn_msg::size) {
            return error(
                    invalid_msg_size,
                    "Received INIT_CONN message of invalid size."
            );
        }
        auto* buf_ptr = msg_buf.data();
        const auto tor_id = read_from_buffer<id_t>(buf_ptr);
        return init_conn_msg(tor_id);
    }

    void conn_res_msg::to_buffer_impl(buffer_ref& msg_buf) const {
		auto* buf_ptr = msg_buf.data();
        for (const bool has_piece : pieces_in_possession) {
            write_to_buffer(has_piece, buf_ptr);
        }
	}

    result<conn_res_msg> conn_res_msg::from_buffer(const const_buffer_ref &msg_buf) {
        if (msg_buf.empty()) {
            return conn_res_msg();
        }
        const auto num_of_pieces = msg_buf.size();
        std::vector<bool> pieces_in_possession(num_of_pieces);
        auto* buf_ptr = msg_buf.data();
        for (auto&& has_piece : pieces_in_possession) {
            has_piece = read_from_buffer<bool>(buf_ptr);
        }
        return conn_res_msg(std::move(pieces_in_possession));
    }

    result<has_piece_msg> has_piece_msg::from_buffer(const const_buffer_ref& msg_buf) {
		if (msg_buf.size() != has_piece_msg::size) {
			return error(
				invalid_msg_size, 
				"Received HAS_PIECE message of invalid size."
			);
		}
		auto* buf_ptr = msg_buf.data();
		const auto piece_idx = read_from_buffer<piece_idx_t>(buf_ptr);
		return has_piece_msg(piece_idx);
	}

	void has_piece_msg::to_buffer_impl(buffer_ref& msg_buf) const {
		auto* buf_ptr = msg_buf.data();
		write_to_buffer(piece_idx, buf_ptr);
	}

	result<request_piece_msg> request_piece_msg::from_buffer(const const_buffer_ref& msg_buf) {
		if (msg_buf.size() != request_piece_msg::size) {
			return error(
				invalid_msg_size,
				"Received REQ_PIECE message of invalid size."
			);
		}
		auto* buf_ptr = msg_buf.data();
		const auto piece_idx = read_from_buffer<piece_idx_t>(buf_ptr);
		return request_piece_msg(piece_idx);
	}

	void request_piece_msg::to_buffer_impl(buffer_ref& msg_buf) const {
		auto* buf_ptr = msg_buf.data();
		write_to_buffer(piece_idx, buf_ptr);
	}

    result<piece_msg> piece_msg::from_buffer(const const_buffer_ref& msg_buf) {
        if (msg_buf.size() < sizeof(piece_idx_t)) {
            return error(
                    invalid_msg_size,
                    "Received PIECE message of invalid size."
            );
        }
        auto* buf_ptr = msg_buf.data();
        const auto piece_idx = read_from_buffer<piece_idx_t>(buf_ptr);
        const auto piece_size = msg_buf.size() - sizeof(piece_idx_t);
        buffer piece_buf(buf_ptr, buf_ptr + piece_size);
        return piece_msg(piece_idx, std::move(piece_buf));
    }

    void piece_msg::to_buffer_impl(buffer_ref &msg_buf) const {
        auto* buf_ptr = msg_buf.data();
        write_to_buffer(piece_idx, buf_ptr);
        for (const auto byte : piece_buf) write_to_buffer(byte, buf_ptr);
    }
}
