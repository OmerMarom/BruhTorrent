#pragma once

#include "result.h"
#include "types.h"
#include <fstream>

namespace bt {
    struct file {
        inline file(std::string in_path, const file_size_t in_size) :
            path(std::move(in_path)),
            size(in_size),
            stream(path)
        { }

        std::string path;
        file_size_t size;
        std::fstream stream;
    };

    namespace disk_io_utils {
        // TODO: Impl.
        inline error write_to_disk(std::fstream& file_stream, buffer data, file_size_t offset)
        { return {}; }
    }
}
