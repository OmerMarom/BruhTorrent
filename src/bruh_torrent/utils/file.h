#pragma once

#include "base/result.h"
#include "base/types.h"
#include <fstream>

namespace bt {
    class BT_API file {
    public:
        static result<file> make(std::string path);

        file(file&& other) noexcept;

        error write(const const_buffer_ref& buf, file_size_t offset);
        error read(buffer_ref& buf, file_size_t offset);

        [[nodiscard]] std::string path() const { return m_path; }
        [[nodiscard]] file_size_t size() const { return m_size; }
        [[nodiscard]] std::fstream& stream() { return m_stream; }

        static constexpr error_code_t open_error = 2;
        static constexpr error_code_t file_size_error = 3;
        static constexpr error_code_t write_error = 4;
        static constexpr error_code_t read_error = 5;

    private:
        explicit file(std::fstream stream, std::string path, file_size_t size);

        std::fstream m_stream;
        std::string m_path;
        file_size_t m_size;
    };
}
