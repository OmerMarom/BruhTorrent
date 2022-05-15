#pragma once

#include "utils/result.h"
#include "utils/types.h"

namespace bt {
    class torrent;

    using file_idx_t = std::size_t;
    using file_size_t = std::size_t;

    struct file_info {
        std::string name;
        file_size_t size;
    };

    using on_write_complete_fn = std::function<void(error)>;

    class disk_io_service {
    public:
        inline disk_io_service(torrent& in_torrent, std::vector<file_info> files) :
            m_torrent(in_torrent),
            m_files(std::move(files))
        { }

        inline disk_io_service(disk_io_service&& other) :
            m_torrent(other.m_torrent),
            m_files(std::move(other.m_files))
        { }

        virtual ~disk_io_service() = default;

        void write(piece_idx_t piece_idx,
                   buffer data,
                   on_write_complete_fn callback);

        static constexpr error_code_t file_idx_out_of_bounds = 1; 

    private:
        result<file_idx_t> get_file_idx(file_size_t offset,
                                        file_size_t& offset_in_file);

        void write_impl(file_idx_t file_idx, file_size_t file_offset,
                        piece_size_t piece_offset, buffer data,
                        on_write_complete_fn callback, error err = error());

        // TODO: Impl.
        inline void write_to_file(file_idx_t file_idx, buffer data, piece_size_t piece_offset,
                                  file_size_t write_size, file_size_t offset_in_file,
                                  on_write_complete_fn callback) { }

        torrent& m_torrent;
        std::vector<file_info> m_files; 
    };
}