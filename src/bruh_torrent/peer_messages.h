#pragma once

#include "base/types.h"

namespace bt { class bt_peer_connection; }

namespace bt::peer_messages {
	using msg_id_t = uint8_t;
    using msg_size_t = uint32_t;
	constexpr error_code_t invalid_msg_id = 2;
	constexpr error_code_t invalid_msg_size = 3;

	error from_buffer(const const_buffer_ref& msg_buf, bt_peer_connection& bt_pc);

	// Read/write helpers:
	template <typename Tdata, typename Tbuf>
	void write_to_buffer(const Tdata& data, Tbuf*& buf_ptr);
	template <typename Tdata, typename Tbuf>
	Tdata read_from_buffer(Tbuf*& buf_ptr);

	struct peer_message {
		virtual ~peer_message() = default;

		[[nodiscard]] buffer to_buffer() const;

		[[nodiscard]] virtual constexpr msg_id_t id() const = 0;
		[[nodiscard]] virtual constexpr const char* name() const = 0;

	protected:
		[[nodiscard]] virtual msg_size_t msg_size() const = 0;
		virtual void to_buffer_impl(buffer_ref& msg_buf) const = 0;
	};

	struct init_conn_msg : peer_message {
		id_t tor_id;

		static constexpr msg_id_t init_conn_msg_id = 1;
        static constexpr msg_size_t size = sizeof(id_t);

        static result<init_conn_msg> from_buffer(const const_buffer_ref& msg_buf);

        explicit init_conn_msg(const id_t t_id) : tor_id(t_id) { }

		[[nodiscard]] constexpr msg_id_t id() const override { return init_conn_msg_id; }
		[[nodiscard]] constexpr const char* name() const override { return "INIT_CONN"; }

	protected:
		[[nodiscard]] msg_size_t msg_size() const override { return sizeof(id_t); }
		void to_buffer_impl(buffer_ref& msg_buf) const override;
	};

	struct conn_res_msg : peer_message {
		// pieces_in_possession.empty() means we don't have torrent.
		std::vector<bool> pieces_in_possession;

		static constexpr msg_id_t conn_res_msg_id = 2;

        static result<conn_res_msg> from_buffer(const const_buffer_ref& msg_buf);

        explicit conn_res_msg(std::vector<bool> pip = {}) :
            pieces_in_possession(std::move(pip))
        { }

        [[nodiscard]] bool has_tor() const { return !pieces_in_possession.empty(); }

        [[nodiscard]] constexpr msg_id_t id() const override { return conn_res_msg_id; }
		[[nodiscard]] constexpr const char* name() const override { return "CONN_RES"; }

	protected:
		[[nodiscard]] msg_size_t msg_size() const override
        { return (msg_size_t)pieces_in_possession.size(); }

		void to_buffer_impl(buffer_ref& msg_buf) const override;
	};

	struct has_piece_msg : peer_message {
		piece_idx_t piece_idx;

		static constexpr msg_id_t msg_id = 3;
		static constexpr msg_size_t size = sizeof(piece_idx_t);

		static result<has_piece_msg> from_buffer(const const_buffer_ref& msg_buf);

		explicit has_piece_msg(const piece_idx_t p_idx) : piece_idx(p_idx) { }

		[[nodiscard]] constexpr msg_id_t id() const override { return msg_id; }
		[[nodiscard]] constexpr const char* name() const override { return "HAS_PIECE"; }

	protected:
		[[nodiscard]] msg_size_t msg_size() const override { return size; }

		void to_buffer_impl(buffer_ref& msg_buf) const override;
	};

	struct request_piece_msg : peer_message {
		piece_idx_t piece_idx;

		static constexpr msg_id_t msg_id = 4;
		static constexpr msg_size_t size = sizeof(piece_idx_t);

		static result<request_piece_msg> from_buffer(const const_buffer_ref& msg_buf);

		explicit request_piece_msg(const piece_idx_t p_idx) : piece_idx(p_idx) { }

		[[nodiscard]] constexpr msg_id_t id() const override { return msg_id; }
		[[nodiscard]] constexpr const char* name() const override { return "REQ_PIECE"; }

	protected:
		[[nodiscard]] msg_size_t msg_size() const override { return size; }
		void to_buffer_impl(buffer_ref& msg_buf) const override;
	};

    struct piece_msg : peer_message {
        piece_idx_t piece_idx;
        buffer piece_buf;
        // TODO: Optim - Add hash field & perform hash-check.

        static constexpr msg_id_t msg_id = 5;

        static result<piece_msg> from_buffer(const const_buffer_ref& msg_buf);

        explicit piece_msg(const piece_idx_t p_idx, buffer p_buf) :
            piece_idx(p_idx),
            piece_buf(std::move(p_buf))
        { }

        piece_msg(piece_msg&& other) noexcept :
            piece_idx(other.piece_idx),
            piece_buf(std::move(other.piece_buf))
        { }

        [[nodiscard]] constexpr msg_id_t id() const override { return msg_id; }
        [[nodiscard]] constexpr const char* name() const override { return "PIECE"; }

    protected:
        [[nodiscard]] msg_size_t msg_size() const override
        { return (msg_size_t)sizeof(piece_idx_t) + (msg_size_t)piece_buf.size(); }

        void to_buffer_impl(buffer_ref& msg_buf) const override;
    };

	// Template impl.
	template <typename Tdata, typename Tbuf>
	void write_to_buffer(const Tdata& data, Tbuf*& buf_ptr) {
		std::memcpy(buf_ptr, &data, sizeof(Tdata));
		buf_ptr += sizeof(Tdata);
	}

	template <typename Tdata, typename Tbuf>
	Tdata read_from_buffer(Tbuf*& buf_ptr) {
		const Tdata data = *(const Tdata*)buf_ptr;
		buf_ptr += sizeof(Tdata);
		return data;
	}
}
