#pragma once

namespace bt::peer_messages {
	template <typename Tdata, typename Tbuf>
	static void write_to_buffer(const Tdata& data, Tbuf*& buf_ptr) {
		std::memcpy(buf_ptr, &data, sizeof(Tdata));
		buf_ptr += sizeof(Tdata);
	}

	using msg_id_t = uint8_t;

	struct peer_message {
		virtual ~peer_message() = default;
		[[nodiscard]] virtual buffer to_buffer() const = 0;
		[[nodiscard]] virtual const char* type() const = 0;
	};

	struct init_conn_msg : peer_message {
		static constexpr msg_id_t id = 1;
		id_t our_id;
		port_t our_port;
		id_t tor_id;

		[[nodiscard]] buffer to_buffer() const override {
			static constexpr std::size_t msg_size = sizeof(msg_id_t) + sizeof(id_t) * 2 + sizeof(port_t);
			buffer buf(msg_size);
			auto* buf_ptr = buf.data();
			write_to_buffer(id, buf_ptr);
			write_to_buffer(our_id, buf_ptr);
			write_to_buffer(our_port, buf_ptr);
			write_to_buffer(tor_id, buf_ptr);
			return buf;
		}

		[[nodiscard]] const char* type() const override { return "INIT_CONN"; }
	};

	struct conn_res_msg : peer_message {
		static constexpr msg_id_t id = 2;
		port_t peer_port;
		std::vector<bool> pieces_in_possession;

		[[nodiscard]] buffer to_buffer() const override {
			const std::size_t msg_size = sizeof(msg_id_t) + sizeof(port_t) + pieces_in_possession.size();
			buffer buf(msg_size);
			auto* buf_ptr = buf.data();
			write_to_buffer(id, buf_ptr);
			write_to_buffer(peer_port, buf_ptr);
			for (const bool has_piece : pieces_in_possession) *(buf_ptr++) = (char)has_piece;
			return buf;
		}

		[[nodiscard]] const char* type() const override { return "CONN_RES"; }
	};

	struct has_piece_msg : peer_message {
		static constexpr msg_id_t id = 3;
		piece_idx_t piece_idx;

		has_piece_msg(const piece_idx_t p_idx) : piece_idx(p_idx) { }
		
		[[nodiscard]] buffer to_buffer() const override {
			static constexpr std::size_t msg_size = sizeof(msg_id_t) + sizeof(piece_idx_t);
			buffer buf(msg_size);
			auto* buf_ptr = buf.data();
			write_to_buffer(id, buf_ptr);
			write_to_buffer(piece_idx, buf_ptr);
			return buf;
		}

		[[nodiscard]] const char* type() const override { return "HAS_PIECE"; }
	};

	struct request_piece_msg : peer_message {
		static constexpr msg_id_t id = 4;
		piece_idx_t piece_idx;

		request_piece_msg(const piece_idx_t p_idx) : piece_idx(p_idx) { }

		[[nodiscard]] buffer to_buffer() const override {
			static constexpr std::size_t msg_size = sizeof(msg_id_t) + sizeof(piece_idx_t);
			buffer buf(msg_size);
			auto* buf_ptr = buf.data();
			write_to_buffer(id, buf_ptr);
			write_to_buffer(piece_idx, buf_ptr);
			return buf;
		}

		[[nodiscard]] const char* type() const override { return "REQUEST_PIECE"; }
	};
}