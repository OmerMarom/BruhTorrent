
#include "peer.h"
#include "torrent.h"
#include "utils/asserts.h"

namespace bt {
    peer::peer(endpoint ep, torrent& in_torrent, std::vector<bool> pieces_in_possession) :
        m_endpoint(std::move(ep)),
        m_torrent(in_torrent),
        m_pieces_in_possession(std::move(pieces_in_possession))
    {
        BT_ASSERT(
            m_pieces_in_possession.size() == in_torrent.num_of_pieces(),
            "peer::peer(): m_pieces_in_possession's size doesn't match torrent.num_of_pieces()"
        );
    }

    peer::peer(endpoint ep, torrent& in_torrent) :
        m_endpoint(std::move(ep)),
        m_torrent(in_torrent),
        m_pieces_in_possession(in_torrent.num_of_pieces(), false)
    { }
}