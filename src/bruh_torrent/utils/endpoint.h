#pragma once

namespace bt {
    using port_t = std::uint16_t;
    
    struct endpoint {
	    endpoint(endpoint&& other) noexcept :
            ip(std::move(other.ip)),
            port(other.port)
        { }

        std::string ip;
        port_t port;
    };
}
