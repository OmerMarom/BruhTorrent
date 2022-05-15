
#include "bt_pch.h"
#include "tracker_service.h"
#include "torrent.h"

namespace bt {
    // TODO: Impl.
    result<torrent> tracker_service::join_torrent_swarm(const endpoint& tracker_endpoint,
                                                        const id_t torrent_id)
    { return error(); }
}