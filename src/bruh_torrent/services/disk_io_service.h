#pragma once

#include "base/result.h"
#include "base/types.h"
#include "utils/file.h"

namespace bt {
    class torrent;
    class disk_executor;

    class BT_API disk_io_service {
    public:
        disk_io_service(torrent& tor, std::vector<file> files, disk_executor& executor);
        disk_io_service(disk_io_service&& other) noexcept;

        void write(piece_idx_t piece_idx, buffer buf, on_error_fn callback);
        void read(piece_idx_t piece_idx, on_result_fn<buffer> callback);

        inline static constexpr error_code_t file_idx_out_of_bounds = 2; 

    private:
        using file_op_fn = std::function<void(file_idx_t, file_size_t, piece_size_t,
                                              file_size_t, on_error_fn)>;

        void pre_file_op(piece_idx_t piece_idx, file_op_fn file_op, on_error_fn callback);
        void pre_file_op_impl(file_idx_t file_idx, file_size_t file_offset,
                              piece_size_t piece_offset, std::shared_ptr<file_op_fn> shared_file_op,
                              on_error_fn callback);

        result<file_idx_t> get_file_idx(file_size_t torrent_offset,
                                        file_size_t& file_offset) const;

        torrent& m_torrent;
        std::vector<file> m_files;
        disk_executor& m_executor;
    };
}