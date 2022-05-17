
#include "torrent_file_parser.h"
#include "utils/endpoint.h"

namespace bt {
	torrent_file_parser::~torrent_file_parser() { }

	// TODO: Impl.
	result<torrent_info> torrent_file_parser::parse(const std::string& torrent_file_path)
	{ return error(); }
}