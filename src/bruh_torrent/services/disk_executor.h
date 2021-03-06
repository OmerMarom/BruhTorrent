#pragma once

#include "base/result.h"
#include "base/types.h"

namespace bt {
    class file;

    class disk_executor {
    public:
        disk_executor();
        ~disk_executor();

        void write(file& f, file_size_t offset, const const_buffer_ref& buf, on_error_fn callback);
        void read(file& f, file_size_t offset, buffer_ref buf, on_error_fn callback);

    private:
        boost::asio::io_context m_io_ctx;
        std::thread m_disk_thread;
    };
}
