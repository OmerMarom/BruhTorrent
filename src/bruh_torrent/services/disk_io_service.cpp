
#include "bt_pch.h"
#include "disk_io_service.h"
#include "torrent.h"
#include <thread>
#include <boost/asio/io_context.hpp>

using namespace std::placeholders;

namespace bt {
    using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    disk_io_service::disk_io_service(torrent& in_torrent, std::vector<file> files) :
        m_torrent(in_torrent),
        m_files(std::move(files)),
        m_io_ctx(std::make_unique<boost::asio::io_context>()) {
        // Start disk thread:
        m_disk_thread = std::make_unique<std::thread>([this]() {
            work_guard_t work_guard(m_io_ctx->get_executor());
            m_io_ctx->run();
        });
    }

    disk_io_service::disk_io_service(disk_io_service&& other) :
        m_torrent(other.m_torrent),
        m_files(std::move(other.m_files)),
        m_io_ctx(std::move(other.m_io_ctx)),
        m_disk_thread(std::move(other.m_disk_thread))
    { }

    disk_io_service::~disk_io_service() {
        // Stop disk thread:
        m_io_ctx->stop();
        if (m_disk_thread->joinable()) m_disk_thread->join();
    }

    void disk_io_service::write(const piece_idx_t piece_idx,
                                buffer data,
                                on_write_complete_fn callback) {
        const auto piece_size = m_torrent.piece_size();
        const auto torrent_offset = (file_size_t)piece_idx * piece_size;
        file_size_t file_offset;
        auto [file_idx, offset_err] = get_file_idx(torrent_offset, file_offset);
        if (offset_err) {
            // TODO: Impl - What should I do with the files that have already been written?
            callback(std::move(offset_err));
        } else {
            write_impl(*file_idx, file_offset, 0, std::move(data), std::move(callback));
        }
    }

    result<file_idx_t> disk_io_service::get_file_idx(const file_size_t offset,
                                                     file_size_t& file_offset) {
        file_size_t file_end_offset = 0;
        for (file_idx_t file_idx = 0; file_idx < m_files.size(); file_idx++) {
            const auto file_size = m_files[file_idx].size;
            file_end_offset += file_size;
            if (offset <= file_end_offset) {
                file_offset = offset - file_end_offset + file_size;
                return { file_idx };
            }
        }
        return { file_idx_out_of_bounds };
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
        const auto file_size = m_files[file_idx].size;
        // Write cant exceed file:
        const auto write_size = std::min(piece_size - piece_offset, file_size - file_offset);
        // Create a copy of the part of the buffer that's going to be written:
        const auto data_begin = data.begin() + piece_offset;
        buffer data_for_this_write(data_begin, data_begin + write_size);
        write_to_file(
            file_idx,
            std::move(data_for_this_write),
            file_offset,
            std::bind(&disk_io_service::write_impl, this,
                file_idx + 1, // next file
                0, // file_offset
                piece_offset + write_size, // piece_offset after next write
                std::move(data),
                std::move(callback),
                _1 // error
            )
        );
    }

    void disk_io_service::write_to_file(file_idx_t file_idx, buffer data, file_size_t offset,
                                        on_write_complete_fn callback) {
        // Perform write on disk thread:
        m_io_ctx->post([this, file_idx, offset, cb = std::move(callback), d = std::move(data)]() {
            error write_err =
                disk_io_utils::write_to_disk(m_files[file_idx].stream, std::move(d), offset);
            cb(std::move(write_err));
        });
    }
}