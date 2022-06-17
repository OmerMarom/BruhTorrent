#pragma once

#include "base/types.h"
#include "base/result.h"

namespace bt {
    class peer_connection;
    class file;
    class alert_service;
    class disk_executor;
    class disk_io_service;

    class BT_API torrent {
    public:
        torrent(id_t id, std::string name,
                piece_idx_t num_of_pieces,
                piece_size_t piece_size,
                std::vector<file> files,
                disk_executor& executor,
                alert_service& alert_service);

        torrent(torrent&& other) noexcept;

        ~torrent();

        [[nodiscard]] id_t id() const { return m_id; }
        [[nodiscard]] piece_idx_t num_of_pieces() const { return (piece_idx_t)m_pieces_in_possession.size(); }
        [[nodiscard]] piece_size_t piece_size() const { return m_piece_size; }
        [[nodiscard]] const std::vector<bool>& pieces_in_possession() const { return m_pieces_in_possession; }

        void set_peer_connections(std::vector<std::unique_ptr<peer_connection>> peers);

        void get_piece(piece_idx_t piece_idx, on_result_fn<buffer> callback) const;
        void on_piece(piece_idx_t piece_idx, buffer piece_buf, on_error_fn callback);

        static constexpr error_code_t piece_not_in_possession = 2;
        static constexpr error_code_t piece_in_possession = 3;
        static constexpr error_code_t invalid_piece_size = 4;
        static constexpr error_code_t invalid_piece_idx = 5;

    private:
        void start_download();

        peer_connection* choose_peer_for_piece(piece_idx_t piece_idx);

        void send_request_piece(piece_idx_t piece_idx);

        void on_has_piece(piece_idx_t piece_idx);

    	bool has_piece(const piece_idx_t piece_idx)
        { return m_pieces_in_possession[piece_idx]; }

        id_t m_id;
        std::string m_name;
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::vector<bool> m_pieces_in_possession;
        piece_size_t m_piece_size;
        std::vector<std::unique_ptr<peer_connection>> m_peers;
        std::unique_ptr<disk_io_service> m_disk_io;
        alert_service& m_alert_service;
    };
}