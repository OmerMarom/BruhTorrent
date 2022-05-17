
#include "bt_pch.h"
#include "peer.h"
#include "utils/endpoint.h"
#include "services/peer_messenger.h"

namespace bt {
    peer::peer(id_t id, endpoint ep, std::vector<bool> pieces_in_possession,
			   boost::asio::io_context& io_ctx, alert_service& as) :
		m_id(id),
		m_messenger(std::make_unique<peer_messenger>(std::move(ep), *this, io_ctx, as)),
        m_pieces_in_possession(std::move(pieces_in_possession)),
		m_alert_service(as)
	{ }

    peer::~peer() { }
}