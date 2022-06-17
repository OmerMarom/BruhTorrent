
#include "base/bt_pch.h"
#include "disk_executor.h"
#include "utils/file.h"

namespace bt {
    using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    disk_executor::disk_executor() :
        m_disk_thread([this] {
            work_guard_t work_guard(m_io_ctx.get_executor());
            m_io_ctx.run();
        })
    { }

    disk_executor::~disk_executor() {
        m_io_ctx.stop();
        if (m_disk_thread.joinable()) m_disk_thread.join();
    }

    void disk_executor::write(file& f, const file_size_t offset, const const_buffer_ref& buf, on_error_fn callback) {
        m_io_ctx.post([buf, &f, offset, cb = std::move(callback)] {
            cb(f.write(buf, offset));
        });
    }

    void disk_executor::read(file &f, file_size_t offset, buffer_ref buf, on_error_fn callback) {
        m_io_ctx.post([buf, &f, offset, cb = std::move(callback)]() mutable {
            cb(f.read(buf, offset));
        });
    }
}