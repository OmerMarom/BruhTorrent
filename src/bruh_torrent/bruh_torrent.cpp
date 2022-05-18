
#include "base/bt_pch.h"
#include "bruh_torrent.h"
#include "torrent.h"
#include "services/torrent_file_parser.h"
#include "services/tracker_service.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "utils/endpoint.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "services/alert_service.h"

namespace bt {
    error bruh_torrent::add_torrent(const std::string& torrent_file_path) {
        // Get torrent info from torrent file:
    	auto tor_info_res = m_torrent_file_parser->parse(torrent_file_path);
        if (!tor_info_res) return tor_info_res.error();
        const auto& [id, tracker_endpoint] = *tor_info_res;
        // Join swarm through provided tracker & torrent id:
        auto new_tor_res =
            m_tracker_service->join_torrent_swarm(*tracker_endpoint, id);
        if (!new_tor_res) return new_tor_res.error();
        auto new_torrent = std::make_unique<torrent>(std::move(*new_tor_res));
        m_torrents.emplace_back(std::move(new_torrent));
        return { };
    }

    bruh_torrent::~bruh_torrent() { }
}