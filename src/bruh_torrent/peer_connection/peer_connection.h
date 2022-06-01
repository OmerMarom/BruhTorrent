#pragma once

#include "base/types.h"

namespace bt {
	class bruh_torrent;
	class torrent;
	class alert_service;

    class BT_API peer_connection BT_ABS_CLASS {
    public:
    	peer_connection(torrent* tor, bruh_torrent& bt, alert_service& as);

        virtual ~peer_connection() = default;

        [[nodiscard]] result<bool> has_piece(piece_idx_t piece_idx) const;

        virtual void request_piece(piece_idx_t piece_idx) = 0;
        virtual void update_has_piece(piece_idx_t piece_idx) = 0;

        static constexpr error_code_t uninitialized_pieces_in_possession = 2;
        static constexpr error_code_t piece_idx_out_of_bounds = 3;

    protected:
        torrent* m_torrent = nullptr;
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::optional<std::vector<bool>> m_pieces_in_possession;
        bruh_torrent& m_bt;
        alert_service& m_alert_service;
    };
}
