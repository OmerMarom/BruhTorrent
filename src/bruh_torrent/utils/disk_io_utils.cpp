
#include "disk_io_utils.h"

namespace bt {
	result<file> file::make(std::string path, const file_size_t size) {
		file f(std::move(path), size);
		if (f.stream().fail()) {
			// TODO: Optim - Provide clearer err message.
			return error(file::open_error, fmt::format("Unable to open file {}.", f.path()));
		}
		return std::move(f);
	}

	file::file(std::string path, const file_size_t size) :
		m_path(std::move(path)),
		m_size(size),
		m_stream(m_path)
	{ }

	file::file(file&& other) noexcept :
		m_path(std::move(other.m_path)),
		m_size(other.m_size),
		m_stream(std::move(other.m_stream))
	{ }

	error file::write(const buffer& data, const file_size_t offset) {
		m_stream.seekg((std::int64_t)offset, std::ios::beg);
		m_stream.write((char*)data.data(), (std::int64_t)data.size());
		if (m_stream.fail()) {
			// TODO: Optim - Provide clearer err message.
			return { file::write_error, fmt::format("Write error for file {}.", m_path) };
		}
		return { };
	}
}