
#include "base/bt_pch.h"
#include "disk_executor.h"
#include "utils/disk_io_utils.h"

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

    void disk_executor::write(buffer buf, file& f, const file_size_t offset, on_write_complete_fn callback) {
        m_io_ctx.post([buf = std::move(buf), &f, offset, cb = std::move(callback)] {
            error write_err = f.write(buf, offset);
            cb(std::move(write_err));
        });
    }
}