
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

    void disk_io_service::write(const piece_idx_t piece_idx,
                                buffer data,
                                on_write_complete_fn callback) {
        const auto piece_size = m_torrent.piece_size();
        const auto torrent_offset = (file_size_t)piece_idx * piece_size;
        file_size_t file_offset;
        auto file_idx_res = get_file_idx(torrent_offset, file_offset);
        if (!file_idx_res) {
            // TODO: Impl - What should I do with the files that have already been written?
            callback(std::move(file_idx_res.error()));
        } else {
            write_impl(*file_idx_res, file_offset, 0, std::move(data), std::move(callback));
        }
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

    void disk_io_service::write_impl(const file_idx_t file_idx, const file_size_t file_offset,
                                     const piece_size_t piece_offset, buffer data,
                                     on_write_complete_fn callback, error err) {
        const auto piece_size = m_torrent.piece_size();
        if (err || piece_offset == piece_size) {
            // TODO: Impl - What should I do with the files that have already been written?
            callback(std::move(err));
            return;
        }
        auto& target_file = m_files[file_idx];
        // Write cant exceed file:
        const auto write_size = std::min(piece_size - piece_offset, target_file.size() - file_offset);
        const auto data_begin = data.begin() + (std::int64_t)piece_offset;
        // Create a copy of the part of the buffer that's going to be written:
        buffer data_for_this_write(data_begin, data_begin + (std::int64_t)write_size);
        const auto write_end_offset = piece_offset + write_size;
        m_executor.write(
            std::move(data_for_this_write),
            target_file,
            file_offset,
            [this, file_idx, write_end_offset, d = std::move(data), cb = std::move(callback)]
            (auto&& err) mutable {
                write_impl(
                        file_idx + 1,
                        0,
                        write_end_offset,
                        std::move(d),
                        std::move(cb),
                        std::forward<decltype(err)>(err)
                );
            }
        );
    }
}
