
#include "bt_pch.h"
#include "bruh_torrent.h"
#include "torrent.h"
#include "utils/endpoint.h"
#include "services/torrent_file_parser.h"
#include "services/tracker_service.h"
#include "services/alert_service.h"

namespace bt {
    error bruh_torrent::add_torrent(const std::string& torrent_file_path) {
        // Get torrent info from torrent file:
        const auto [tor_info, parse_err] = m_torrent_file_parser->parse(torrent_file_path);
        if (parse_err) return parse_err;
        // Join swarm through provided tracker & torrent id:
        auto [new_torrent, tracker_err] =
            m_tracker_service->join_torrent_swarm(*tor_info->tracker_endpoint, tor_info->id);
        if (tracker_err) return tracker_err;
        m_torrents.emplace_back(std::move(new_torrent));
        return { };
    }

    bruh_torrent::~bruh_torrent() { }
}