#pragma once

#include <string>

namespace bt {

    using error_code_t = std::uint8_t;

    struct error {
        error() = default;

        inline error(error_code_t err_code, std::string msg = {}) :
            code(err_code),
            message(std::move(msg))
        { }

        inline error(const error& other) :
            code(other.code),
            message(other.message)
        { }

        inline error(error&& other) :
            code(other.code),
            message(std::move(other.message))
        { }

        virtual ~error() = default;

        static constexpr error_code_t no_error = 0;

        inline error operator=(const error& other) {
            code = other.code,
            message = other.message;
            return *this;
        }

        inline operator bool() const {
            return code != no_error;
        }

        error_code_t code = no_error;
        std::string message;
    };

    template <typename Tpayload>
    struct result {
        inline result(Tpayload pl, error e) :
            payload(std::move(pl)),
            error(std::move(e))
        { };

        inline result(Tpayload pl) : payload(std::move(pl)) { };

        inline result(error e) : error(std::move(e)) { };

        inline result(result<Tpayload>&& other) : 
            payload(std::move(other.payload)),
            error(std::move(other.error))
        { };

        virtual ~result() = default;

        Tpayload payload;
        error error;
    };
}