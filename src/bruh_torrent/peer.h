#pragma once

#include "services/disk_io_service.h"
#include "utils/endpoint.h"
#include <functional>

namespace bt {
    class torrent;

    using on_piece_fn = std::function<void(buffer, error)>;

    class peer {
    public:
        inline peer(endpoint ep, torrent& in_torrent, std::vector<bool> pieces_in_possession);

        inline peer(endpoint ep, torrent& in_torrent);

        virtual ~peer() = default;

        inline bool has_piece(const piece_idx_t piece_idx)
        { return m_pieces_in_possession[piece_idx]; }

        // TODO: Impl.
        void send_request_piece(piece_idx_t piece_idx, on_piece_fn callback);

        // TODO: Impl.
        void send_has_piece(piece_idx_t piece_idx);
    
    private:
        endpoint m_endpoint;
        torrent& m_torrent; // Peer should never outlive associated torrent.
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::vector<bool> m_pieces_in_possession; 
    };
}