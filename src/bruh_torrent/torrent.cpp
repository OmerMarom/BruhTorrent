
#include "base/bt_pch.h"
#include "torrent.h"
#include "peer_connection/peer_connection.h"
#include "services/disk_io_service.h"
#include "services/alert_service.h"

using namespace std::placeholders;

namespace bt {
    torrent::torrent(const id_t id,  std::string name,
                     const piece_idx_t num_of_pieces,
                     const piece_size_t piece_size,
                     std::vector<file> files,
                     disk_executor& executor,
                     alert_service& alert_service) :
        m_id(id),
        m_name(std::move(name)),
        m_pieces_in_possession(num_of_pieces, false),
        m_piece_size(piece_size),
        m_disk_io(std::make_unique<disk_io_service>(*this, std::move(files), executor)),
        m_alert_service(alert_service) {
        start_download();
    }

    torrent::torrent(torrent&& other) noexcept :
        m_id(other.m_id),
        m_name(std::move(other.m_name)),
        m_pieces_in_possession(std::move(other.m_pieces_in_possession)),
        m_piece_size(other.m_piece_size),
        m_peers(std::move(other.m_peers)),
        m_disk_io(std::move(other.m_disk_io)),
        m_alert_service(other.m_alert_service)
    { }

    torrent::~torrent() = default;

    void torrent::set_peer_connections(std::vector<std::unique_ptr<peer_connection>> peers)
    { m_peers = std::move(peers); }

    void torrent::start_download() {
        const auto num_pieces = m_pieces_in_possession.size();
        for (piece_idx_t piece_idx = 0; piece_idx < num_pieces; piece_idx++) {
            if (!has_piece(piece_idx)) send_request_piece(piece_idx);
        }
    }

    void torrent::send_request_piece(const piece_idx_t piece_idx) {
        if (peer_connection* p = choose_peer_for_piece(piece_idx)) {
            p->request_piece(piece_idx);
        } else {
            // TODO: Impl - Handle no peer_connection has this piece.
        }
    }

    // TODO: Optim - Currently this function just returns the first peer_connection that has the piece.
    //       Should impl. something a bit more nuanced... (piece rarity, peer_connection bandwidth, etc...)
    peer_connection* torrent::choose_peer_for_piece(const piece_idx_t piece_idx) {
        for (auto& peer : m_peers) {
            if (peer->has_piece(piece_idx)) return peer.get();
        }
        return nullptr;
    }

    void torrent::get_piece(const piece_idx_t piece_idx, on_result_fn<buffer> callback) const {
        if (piece_idx >= num_of_pieces()) {
            callback(error(
                invalid_piece_idx,
                fmt::format("Received invalid piece idx {} in torrent {}.", piece_idx, m_name)
            ));
        } else if (!m_pieces_in_possession.at(piece_idx)) {
            callback(error(
                piece_not_in_possession,
                fmt::format("Piece {} of torrent {} not in possession.", piece_idx, m_name)
            ));
        } else {
            m_disk_io->read(piece_idx, std::move(callback));
        }
    }

    void torrent::on_piece(const piece_idx_t piece_idx, buffer piece_buf, on_error_fn callback) {
        if (piece_idx >= num_of_pieces()) {
            callback({
                invalid_piece_idx,
                fmt::format("Received invalid piece idx {} in torrent {}.", piece_idx, m_name)
            });
        } else if (m_pieces_in_possession.at(piece_idx)) {
            callback({
                piece_in_possession,
                fmt::format("Piece {} of torrent {} already in possession.", piece_idx, m_name)
            });
        } else if (piece_buf.size() != m_piece_size) {
            callback({
                invalid_piece_size,
                fmt::format(
                    "Received piece of invalid size in torrent {}. Expected/actual: {}/{}.",
                    m_name, m_piece_size, piece_buf.size()
                )
            });
        } else {
            m_disk_io->write(
                piece_idx, std::move(piece_buf),
                [this, piece_idx, cb = std::move(callback)](error&& err) {
                    if (!err) on_has_piece(piece_idx);
                    cb(std::forward<error>(err));
                }
            );
        }
    }

    // TODO: Optim - send has_piece only to the peers that don't have said piece.
    void torrent::on_has_piece(const piece_idx_t piece_idx) {
        m_pieces_in_possession[piece_idx] = true;
        for (auto& p : m_peers) p->update_has_piece(piece_idx);
    }
}