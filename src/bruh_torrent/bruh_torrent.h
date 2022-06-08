#pragma once

#include "base/result.h"
#include "base/types.h"

namespace bt {
    class torrent;
    class torrent_file_parser;
    class tracker_service;
    class alert_service;

    class bruh_torrent {
    public:
        ~bruh_torrent();

        error add_torrent(const std::string& torrent_file_path);

        torrent* get_torrent(id_t tor_id);

    private:
        std::unordered_map<id_t, std::unique_ptr<torrent>> m_torrents;
        std::unique_ptr<torrent_file_parser> m_torrent_file_parser;
        std::unique_ptr<tracker_service> m_tracker_service;
        std::unique_ptr<alert_service> m_alert_service;
    };
}