#pragma once

#include "base/result.h"
#include "base/types.h"
#include <fstream>

namespace bt {
    class BT_API file {
    public:
        static result<file> make(std::string path, file_size_t size);

        file(std::string path, file_size_t size);

        file(file&& other) noexcept;

        virtual ~file() = default;

        error write(const buffer& data, file_size_t offset);

        static constexpr error_code_t open_error = 1;
        static constexpr error_code_t write_error = 1;

        [[nodiscard]] std::string path() const { return m_path; }

        [[nodiscard]] file_size_t size() const { return m_size; }

    	std::fstream& stream() { return m_stream; }

    private:
        std::string m_path;
        file_size_t m_size;
        std::fstream m_stream;
    };
}
