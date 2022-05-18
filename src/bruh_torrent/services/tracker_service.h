#pragma once

#include "base/result.h"
#include "base/types.h"

namespace bt {
    class torrent;
    struct endpoint;

    class BT_API tracker_service {
    public:
        tracker_service() = default;

        virtual ~tracker_service();

        tracker_service(const tracker_service&) = default;
        tracker_service(tracker_service&&) = default;
        tracker_service& operator=(const tracker_service&) = default;
        tracker_service& operator=(tracker_service&&) = default;

        result<torrent> join_torrent_swarm(const endpoint& tracker_endpoint, id_t torrent_id);
    };
}