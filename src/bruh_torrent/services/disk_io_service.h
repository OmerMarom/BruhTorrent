#pragma once

#include "utils/result.h"
#include "utils/types.h"
#include "utils/disk_io_utils.h"

namespace std { class thread; }
namespace boost::asio { class io_context; }

namespace bt {
    class torrent;

    using on_write_complete_fn = std::function<void(error)>;

    class disk_io_service {
    public:
        disk_io_service(torrent& in_torrent, std::vector<file> files);

        disk_io_service(disk_io_service&& other);

        virtual ~disk_io_service();

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

        void write_to_file(file_idx_t file_idx, buffer data, file_size_t offset_in_file,
                           on_write_complete_fn callback);

        torrent& m_torrent;
        std::unique_ptr<std::thread> m_disk_thread;
        std::unique_ptr<boost::asio::io_context> m_io_ctx;
        std::vector<file> m_files;
    };
}