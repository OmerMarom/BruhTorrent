
#include "file.h"
#include <filesystem>

namespace bt {
	result<file> file::make(std::string path) {
        std::fstream stream(path);
		if (stream.fail()) {
			// TODO: Optim - Provide clearer err message.
			return error(file::open_error, fmt::format("Unable to open file {}.", path));
		}
        std::error_code ec;
        const auto file_size = std::filesystem::file_size(path, ec);
        if (ec) {
            return error(
                file::file_size_error,
                fmt::format("Unable to get file size: {}", ec.message())
            );
        }
		return file(std::move(stream), std::move(path), file_size);
	}

	file::file(std::fstream stream, std::string path, const file_size_t size) :
        m_stream(std::move(stream)),
        m_path(std::move(path)),
        m_size(size)
    { }

	file::file(file&& other) noexcept :
        m_stream(std::move(other.m_stream)),
        m_path(std::move(other.m_path)),
		m_size(other.m_size)
	{ }

	error file::write(const const_buffer_ref& buf, const file_size_t offset) {
		m_stream.seekg((std::int64_t)offset, std::ios::beg);
		m_stream.write((char*)buf.data(), (std::int64_t)buf.size());
		if (m_stream.fail()) {
			// TODO: Optim - Provide clearer err message.
			return { file::write_error, fmt::format("Write error for file {}.", m_path) };
		}
		return { };
	}

    error file::read(buffer_ref &buf, file_size_t offset) {
        m_stream.seekg((std::int64_t)offset, std::ios::beg);
        m_stream.read((char*)buf.data(), buf.size());
        if (m_stream.fail()) {
            // TODO: Optim - Provide clearer err message.
            return { file::read_error, fmt::format("Read error for file {}.", m_path) };
        }
        return { };
    }
}