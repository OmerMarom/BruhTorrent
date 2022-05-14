#pragma once

#include "torrent.h"
#include <vector>

namespace bt {
    class tracker_service {
    public:
        tracker_service() = default;

        virtual ~tracker_service() = default;

        result<torrent> join_torrent_swarm(const endpoint& tracker_endpoint,
                                           id_t torrent_id);
    };
}