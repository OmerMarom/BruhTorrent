#pragma once

#include "utils/types.h"

namespace bt {
	class alert_service;
	struct endpoint;
    class peer_messenger;

    class BT_API peer {
    public:
    	peer(id_t id, endpoint ep, std::vector<bool> pieces_in_possession,
             boost::asio::io_context& io_ctx, alert_service& as);

        virtual ~peer();

        peer_messenger& messenger() const { return *m_messenger; }

    	bool has_piece(const piece_idx_t piece_idx)
        { return m_pieces_in_possession[piece_idx]; }

        id_t id() const { return m_id; }

    private:
        id_t m_id;
        std::unique_ptr<peer_messenger> m_messenger;
        // TODO: Optim - make m_pieces_in_possession a bitfield.
        std::vector<bool> m_pieces_in_possession;
        alert_service& m_alert_service;
    };
}