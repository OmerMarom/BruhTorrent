
#include "base/bt_pch.h"
#include "tracker_service.h"
#include "torrent.h"
#include "utils/endpoint.h"
#include "peer_connection/bt_peer_connection.h"
#include "utils/file.h"
#include "disk_executor.h"

namespace bt {
    // TODO: Impl.
    result<torrent> tracker_service::join_torrent_swarm(const endpoint& tracker_endpoint,
                                                        const id_t torrent_id) {
        static constexpr const char* tor_name = "bruh";
        static constexpr piece_idx_t num_of_pieces = 5;
        static constexpr piece_size_t piece_size = 200;

        auto r_file = file::make("C:/some_tor_file.txt");
        std::vector<bt::file> files;
        files.emplace_back(std::move(*r_file));
        torrent tor(
            torrent_id,
            tor_name,
            num_of_pieces,
            piece_size,
            std::move(files),
            m_executor,
            m_alert
        );
        auto peer = std::make_unique<bt_peer_connection>(tor, m_bt, m_alert, endpoint("0.0.0.0", 5000), m_io_ctx);
        std::vector<std::unique_ptr<peer_connection>> peers;
        peers.emplace_back(std::move(peer));
        tor.set_peer_connections(std::move(peers));
        return { std::move(tor) };
    }
}