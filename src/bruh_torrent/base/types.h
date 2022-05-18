#pragma once

#include "result.h"

namespace bt {
	using id_t = std::int64_t;

	using piece_idx_t = std::size_t;
	using piece_size_t = std::size_t;

	using file_idx_t = std::size_t;
	using file_size_t = std::size_t;

	using buffer = std::vector<char>;

	using on_piece_fn = std::function<void(buffer, error)>;
}
