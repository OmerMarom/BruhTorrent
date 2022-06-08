
#include "base/bt_pch.h"
#include "bruh_torrent.h"
#include "torrent.h"
#include "services/torrent_file_parser.h"
#include "services/tracker_service.h"
#include "services/alert_service.h"
#include "utils/endpoint.h"

namespace bt {
    bruh_torrent::~bruh_torrent() = default;

    error bruh_torrent::add_torrent(const std::string& torrent_file_path) {
        // TODO: Impl - Check torrent doesn't already exist.
        // Get torrent info from torrent file:
    	auto tor_info_res = m_torrent_file_parser->parse(torrent_file_path);
        if (!tor_info_res) return tor_info_res.error();
        const auto& [id, tracker_endpoint] = *tor_info_res;
        // Join swarm through provided tracker & torrent id:
        auto new_tor_res =
            m_tracker_service->join_torrent_swarm(*tracker_endpoint, id);
        if (!new_tor_res) return new_tor_res.error();
        auto new_torrent = std::make_unique<torrent>(std::move(*new_tor_res));
        m_torrents[id] = std::move(new_torrent);
        return { };
    }

    torrent* bruh_torrent::get_torrent(const id_t tor_id) {
        return m_torrents.contains(tor_id) ? m_torrents[tor_id].get() : nullptr;
    }
}