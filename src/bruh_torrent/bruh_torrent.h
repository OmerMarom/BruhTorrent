#pragma once

#include "torrent.h"
#include "services/torrent_file_parser.h"
#include "services/tracker_service.h"
#include "services/alert_service.h"

namespace bt {
    class bruh_torrent {
    public:
        bruh_torrent() = default;

        virtual ~bruh_torrent() = default;

        error add_torrent(const std::string& file_path);

    private:
        std::vector<torrent> m_torrents;
        torrent_file_parser m_torrent_file_parser;
        tracker_service m_tracker_service;
        alert_service m_alert_service;
    };
}