#pragma once

#include "peer.h"
#include "services/disk_io_service.h"
#include "services/alert_service.h"
#include "utils/result.h"
#include "utils/id_utils.h"
#include <vector>

namespace bt {
    class torrent {
    public:
        inline torrent(const id_t id, std::vector<peer> peers,
                       const piece_idx_t num_of_pieces,
                       const piece_size_t piece_size,
                       std::vector<file_info> files,
                       alert_service& alert_service) :
            m_id(id),
            m_peers(std::move(peers)),
            m_pieces_in_possession(num_of_pieces, false),
            m_piece_size(piece_size),
            m_alert_service(alert_service),
            m_disk_io(*this, std::move(files)) {
            start_download();
        }

        inline torrent(torrent&& other) :
            m_id(other.m_id),
            m_peers(std::move(other.m_peers)),
            m_pieces_in_possession(std::move(other.m_pieces_in_possession)),
            m_piece_size(other.m_piece_size),
            m_alert_service(other.m_alert_service),
            m_disk_io(std::move(other.m_disk_io))
        { }

        virtual ~torrent() = default;

        inline piece_idx_t num_of_pieces() { return m_pieces_in_possession.size(); }

        inline piece_size_t piece_size() { return m_piece_size; }

    private:
        void start_download();

        peer* choose_peer_for_piece(piece_idx_t piece_idx);

        void send_request_piece(piece_idx_t piece_idx);

        void send_has_piece(piece_idx_t piece_idx);

        void on_piece(piece_idx_t piece_idx, buffer buf, error err);

        void on_piece_write_complete(piece_idx_t piece_idx, error err);

        inline bool has_piece(const piece_idx_t piece_idx)
        { return m_pieces_in_possession[piece_idx]; }

        id_t m_id;
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::vector<bool> m_pieces_in_possession;
        piece_size_t m_piece_size;
        std::vector<peer> m_peers;
        disk_io_service m_disk_io;
        alert_service& m_alert_service;
    };
}