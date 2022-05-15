#pragma once

#include "utils/result.h"
#include "utils/types.h"

namespace bt {
    class torrent;
    struct endpoint;

    class tracker_service {
    public:
        tracker_service() = default;

        virtual ~tracker_service() = default;

        result<torrent> join_torrent_swarm(const endpoint& tracker_endpoint, id_t torrent_id);
    };
}