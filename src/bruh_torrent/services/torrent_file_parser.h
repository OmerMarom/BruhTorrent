#pragma once

#include "utils/id_utils.h"
#include "utils/result.h"

namespace bt {
    struct endpoint;

    struct torrent_info {
        id_t id;
        std::unique_ptr<endpoint> tracker_endpoint;
    };

    class torrent_file_parser {
    public:
        torrent_file_parser() = default;

        virtual ~torrent_file_parser() = default;

        // TODO: Impl.
        inline result<torrent_info> parse(const std::string& torrent_file_path)
        { return { error::no_error }; }
    };
}