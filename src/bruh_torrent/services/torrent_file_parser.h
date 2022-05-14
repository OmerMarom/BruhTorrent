#pragma once

namespace bt {
    struct torrent_info {
        id_t id;
        endpoint tracker_endpoint;
    };

    class torrent_file_parser {
    public:
        torrent_file_parser() = default;

        virtual ~torrent_file_parser() = default;

        result<torrent_info> parse(const std::string& torrent_file_path);
    };
}