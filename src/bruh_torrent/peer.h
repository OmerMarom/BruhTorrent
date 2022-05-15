#pragma once

#include "utils/types.h"
#include "utils/result.h"

namespace bt {
    class torrent;
    struct endpoint;

    using on_piece_fn = std::function<void(buffer, error)>;

    class peer {
    public:
        inline peer(std::unique_ptr<endpoint> ep, torrent& in_torrent, std::vector<bool> pieces_in_possession);

        inline peer(std::unique_ptr<endpoint> ep, torrent& in_torrent);

        virtual ~peer() = default;

        inline bool has_piece(const piece_idx_t piece_idx)
        { return m_pieces_in_possession[piece_idx]; }

        // TODO: Impl.
        inline void send_request_piece(piece_idx_t piece_idx, on_piece_fn callback) { }

        // TODO: Impl.
        inline void send_has_piece(piece_idx_t piece_idx) { }
    
    private:
        std::unique_ptr<endpoint> m_endpoint;
        torrent& m_torrent; // Peer should never outlive associated torrent.
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::vector<bool> m_pieces_in_possession; 
    };
}