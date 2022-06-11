#pragma once

#include "base/result.h"
#include "base/types.h"

namespace bt {
    class file;

    using on_write_complete_fn = std::function<void(error)>;

    class disk_executor {
    public:
        disk_executor();
        ~disk_executor();

        void write(buffer buf, file& f, file_size_t offset, on_write_complete_fn callback);

    private:
        boost::asio::io_context m_io_ctx;
        std::thread m_disk_thread;
    };
}
