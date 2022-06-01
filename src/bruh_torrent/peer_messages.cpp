
#include "peer_messages.h"
#include "peer_connection/bt_peer_connection.h"

namespace bt::peer_messages {
	msg_id_t get_msg_id(const const_buffer_ref& msg_buf) {
		return *(const msg_id_t*)msg_buf.data();
	}

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
			}
			case request_piece_msg::msg_id: {
				auto r_req_piece = request_piece_msg::from_buffer(msg_buf_ref);
				if (!r_req_piece) return r_req_piece.error();
				bt_pc.on_req_piece(*r_req_piece);
				break;
			}
			default: {
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

	void conn_res_msg::to_buffer_impl(buffer_ref& msg_buf) const {
		auto* buf_ptr = msg_buf.data();
		const bool has_tor = pieces_in_possession.has_value();
		write_to_buffer(has_tor, buf_ptr);
		if (has_tor) {
			for (const bool has_piece : *pieces_in_possession) {
				write_to_buffer(has_piece, buf_ptr);
			}
		}
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
}
