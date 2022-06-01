#pragma once

#include "base/types.h"

namespace bt {
    struct endpoint {
        endpoint(std::string ip, port_t port) :
            ip(std::move(ip)),
            port(port)
        { }

	    endpoint(endpoint&& other) noexcept :
            ip(std::move(other.ip)),
            port(other.port)
        { }

        std::string ip;
        port_t port;
    };
}
