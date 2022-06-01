
#include "base/bt_pch.h"
#include "peer_connection.h"

#include "torrent.h"

namespace bt {
    peer_connection::peer_connection(torrent* tor, alert_service& as) :
		m_torrent(tor),
		m_alert_service(as)
	{ }

    result<bool> peer_connection::has_piece(const piece_idx_t piece_idx) const {
		if (!m_pieces_in_possession) {
			return error(
				uninitialized_pieces_in_possession,
				"Querying uninitialized pieces in possession."
			);
		}
    	if (piece_idx > m_pieces_in_possession->size()) {
			return error(
				piece_idx_out_of_bounds,
				"Passing invalid piece index for pieces in possession query."
			);
		}
		return m_pieces_in_possession->at(piece_idx);
    }
}
