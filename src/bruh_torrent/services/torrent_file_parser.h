#pragma once

#include "utils/id_utils.h"
#include "base/result.h"

namespace bt {
    struct endpoint;

    struct torrent_info {
        id_t id;
        std::unique_ptr<endpoint> tracker_endpoint;
    };

    class BT_API torrent_file_parser {
    public:
        torrent_file_parser() = default;

        virtual ~torrent_file_parser();

        result<torrent_info> parse(const std::string& torrent_file_path);
    };
}