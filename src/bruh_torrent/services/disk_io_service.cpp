
#include "base/bt_pch.h"
#include "disk_io_service.h"
#include "torrent.h"
#include "disk_executor.h"

using namespace std::placeholders;

namespace bt {
    disk_io_service::disk_io_service(torrent& tor, std::vector<file> files, disk_executor& executor) :
        m_torrent(tor),
        m_files(std::move(files)),
        m_executor(executor)
    { }

    disk_io_service::disk_io_service(disk_io_service&& other) noexcept :
        m_torrent(other.m_torrent),
        m_files(std::move(other.m_files)),
        m_executor(other.m_executor)
    { }

    void disk_io_service::write(const piece_idx_t piece_idx, buffer buf, on_error_fn callback) {
        pre_file_op(
            piece_idx,
            [this, buf = std::move(buf)]
            (auto file_idx, auto file_offset, auto piece_offset, auto op_size, auto callback) {
                m_executor.write(
                    m_files[file_idx],
                    file_offset,
                    { buf.begin() + piece_offset, op_size },
                    std::move(callback)
                );
            },
            std::move(callback)
        );
    }

    void disk_io_service::read(const piece_idx_t piece_idx, on_result_fn<buffer> callback) {
        auto shared_read_buf = std::make_shared<buffer>(m_torrent.piece_size());
        pre_file_op(
                piece_idx,
                [this, shared_read_buf]
                (auto file_idx, auto file_offset, auto piece_offset, auto op_size, auto callback) mutable {
                    m_executor.read(
                        m_files[file_idx],
                        file_offset,
                        { shared_read_buf->begin() + piece_offset, op_size },
                        std::move(callback)
                    );
                },
                [shared_read_buf, callback = std::move(callback)](error&& err) mutable {
                    if (err) {
                        callback(std::forward<error>(err));
                    } else {
                        callback(std::move(*shared_read_buf));
                    }
                }
        );
    }

    void disk_io_service::pre_file_op(const piece_idx_t piece_idx, file_op_fn file_op, on_error_fn callback) {
        file_size_t file_offset;
        const auto torrent_offset = (file_size_t)piece_idx * m_torrent.piece_size();
        auto r_file_idx = get_file_idx(torrent_offset, file_offset);
        if (!r_file_idx) {
            callback(std::move(r_file_idx.error()));
            return;
        }
        pre_file_op_impl(
            *r_file_idx,
            file_offset,
            0,
            std::make_shared<file_op_fn>(std::move(file_op)),
            std::move(callback)
        );
    }

    void disk_io_service::pre_file_op_impl(const file_idx_t file_idx, const file_size_t file_offset,
                                           const piece_size_t piece_offset, std::shared_ptr<file_op_fn> shared_file_op,
                                           on_error_fn callback) {
        const auto piece_size = m_torrent.piece_size();
        if (piece_offset == piece_size) {
            callback({});
            return;
        }
        // Single op can't exceed file:
        const auto op_size = std::min(piece_size - piece_offset, m_files[file_idx].size() - file_offset);
        const auto new_piece_offset = piece_offset + op_size;
        (*shared_file_op)(
            file_idx,
            file_offset,
            piece_offset,
            op_size,
            [this, file_idx, new_piece_offset, shared_file_op, callback = std::move(callback)]
            (error err) mutable {
                if (err) {
                    callback(std::move(err));
                } else {
                    pre_file_op_impl(file_idx + 1, 0, new_piece_offset,
                                     std::move(shared_file_op), std::move(callback));
                }
            }
        );
    }

    result<file_idx_t> disk_io_service::get_file_idx(const file_size_t torrent_offset,
                                                     file_size_t& file_offset) const {
        file_size_t file_end_offset = 0;
        file_idx_t file_idx = 0;
        for (const auto& f : m_files) {
            const auto file_size = f.size();
            file_end_offset += file_size;
            if (torrent_offset <= file_end_offset) {
                file_offset = torrent_offset - file_end_offset + file_size;
                return file_idx;
            }
            file_idx++;
        }
        return error(file_idx_out_of_bounds);
    }
}
