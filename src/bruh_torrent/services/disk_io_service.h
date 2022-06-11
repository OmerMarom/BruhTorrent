#pragma once

#include "base/result.h"
#include "base/types.h"
#include "utils/disk_io_utils.h"

namespace bt {
    class torrent;
    class disk_executor;

    using on_write_complete_fn = std::function<void(error)>;

    class BT_API disk_io_service {
    public:
        disk_io_service(torrent& tor, std::vector<file> files, disk_executor& executor);
        disk_io_service(disk_io_service&& other) noexcept;

        void write(piece_idx_t piece_idx,
                   buffer data,
                   on_write_complete_fn callback);

        inline static constexpr error_code_t file_idx_out_of_bounds = 2; 

    private:
        result<file_idx_t> get_file_idx(file_size_t torrent_offset,
                                        file_size_t& file_offset) const;

        void write_impl(file_idx_t file_idx, file_size_t file_offset,
                        piece_size_t piece_offset, buffer data,
                        on_write_complete_fn callback, error err = {});

        torrent& m_torrent;
        std::vector<file> m_files;
        disk_executor& m_executor;
    };
}