#pragma once

#include "base/result.h"
#include "base/types.h"
#include <fstream>

namespace bt {
    class BT_API file {
    public:
        static result<file> make(std::string path, file_size_t size);

        file(file&& other) noexcept;

        error write(const buffer& data, file_size_t offset);

        [[nodiscard]] std::string path() const { return m_path; }
        [[nodiscard]] file_size_t size() const { return m_size; }
        [[nodiscard]] std::fstream& stream() { return m_stream; }

        static constexpr error_code_t open_error = 2;
        static constexpr error_code_t write_error = 3;

    private:
        file(std::string path, file_size_t size);

        std::string m_path;
        file_size_t m_size;
        std::fstream m_stream;
    };
}
