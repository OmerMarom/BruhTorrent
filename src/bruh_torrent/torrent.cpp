
#include "base/bt_pch.h"
#include "torrent.h"
#include "peer.h"
#include "services/disk_io_service.h"
#include "services/alert_service.h"
#include "services/peer_messenger.h"

using namespace std::placeholders;

namespace bt {
    torrent::torrent(const id_t id, std::vector<peer> peers,
                     const piece_idx_t num_of_pieces,
                     const piece_size_t piece_size,
                     std::vector<file> files,
                     alert_service& alert_service) :
        m_id(id),
        m_pieces_in_possession(num_of_pieces, false),
        m_piece_size(piece_size),
        m_peers(std::move(peers)),
        m_disk_io(std::make_unique<disk_io_service>(*this, std::move(files))),
        m_alert_service(alert_service) {
        start_download();
    }

    torrent::torrent(torrent&& other) noexcept :
        m_id(other.m_id),
        m_pieces_in_possession(std::move(other.m_pieces_in_possession)),
        m_piece_size(other.m_piece_size),
        m_peers(std::move(other.m_peers)),
        m_disk_io(std::move(other.m_disk_io)),
        m_alert_service(other.m_alert_service)
    { }

    torrent::~torrent() { }

    void torrent::start_download() {
        const auto num_pieces = m_pieces_in_possession.size();
        for (piece_idx_t piece_idx = 0; piece_idx < num_pieces; piece_idx++) {
            if (!has_piece(piece_idx)) send_request_piece(piece_idx);
        }
    }

    void torrent::send_request_piece(const piece_idx_t piece_idx) {
        if (peer* p = choose_peer_for_piece(piece_idx)) {
            /*p->send_request_piece(
                piece_idx,
                std::bind(&torrent::on_piece, this, piece_idx, _1, _2)
            );*/
        } else {
            // TODO: Impl - Handle no peer has this piece.
        }
    }

    // TODO: Optim - Currently this function just returns the first peer that has the piece.
    //       Should impl. something a bit more nuanced... (piece rarity, peer bandwidth, etc...)
    peer* torrent::choose_peer_for_piece(const piece_idx_t piece_idx) {
        for (auto& peer : m_peers) {
            if (peer.has_piece(piece_idx)) return &peer;
        }
        return nullptr;
    }

    void torrent::on_piece(const piece_idx_t piece_idx, buffer data, const error& err) {
        if (err) {
            m_alert_service.notify_error(err);
            // TODO: Impl - Try another peer to get the piece from.
        } else {
            m_disk_io->write(
                piece_idx, std::move(data),
                std::bind(&torrent::on_piece_write_complete, this, piece_idx, _1)
            );
        }
    }

    void torrent::on_piece_write_complete(const piece_idx_t piece_idx, const error& err) {
        if (err) {
            m_alert_service.notify_error(err);
            // TODO: Impl - Abort torrent.
        } else {
            m_pieces_in_possession[piece_idx] = true;
            send_has_piece(piece_idx);
        }
    }

    // TODO: Optim - send has_piece only to the peers that don't have said piece.
    void torrent::send_has_piece(const piece_idx_t piece_idx) {
        for (peer& p : m_peers) {
            p.messenger().send_has_piece({ piece_idx });
        }
    }
}