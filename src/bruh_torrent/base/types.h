#pragma once

#include "result.h"

namespace bt {
	using id_t = std::int64_t;

	using port_t = std::uint16_t;

	using piece_idx_t = std::uint32_t;
	using piece_size_t = std::uint64_t;

	using file_idx_t = std::uint16_t;
	using file_size_t = std::uint64_t;

	using buffer = std::vector<std::int8_t>;
	using buffer_ref = std::span<std::int8_t>;
	using const_buffer_ref = std::span<const std::int8_t>;
}
