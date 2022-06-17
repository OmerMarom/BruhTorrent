#pragma once

#include "base/result.h"
#include "base/types.h"
#include "alert_service.h"

namespace bt {
    class torrent;
    struct endpoint;
    class disk_executor;
    class bruh_torrent;

    class BT_API tracker_service {
    public:
        explicit tracker_service(bruh_torrent& bt,
                                 boost::asio::io_context& io_ctx,
                                 disk_executor& executor,
                                 alert_service& alert) :
            m_bt(bt),
            m_io_ctx(io_ctx),
            m_executor(executor),
            m_alert(alert)
        { }

        result<torrent> join_torrent_swarm(const endpoint& tracker_endpoint, id_t torrent_id);

    private:
        bruh_torrent& m_bt;
        boost::asio::io_context& m_io_ctx;
        disk_executor& m_executor;
        alert_service& m_alert;
    };
}