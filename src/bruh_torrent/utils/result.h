#pragma once

namespace bt {

    using error_code_t = std::uint8_t;

    struct error {
        error() = default;

    	error(const error_code_t err_code, std::string msg = {}) :
            code(err_code),
            message(std::move(msg))
        { }

    	error(const error& other) :
            code(other.code),
            message(other.message)
        { }

    	error(error&& other) noexcept :
            code(other.code),
            message(std::move(other.message))
        { }

        virtual ~error() = default;

        static constexpr error_code_t no_error = 0;

    	error& operator=(const error& other) {
            code = other.code;
            message = other.message;
            return *this;
        }

        explicit operator bool() const {
            return code != no_error;
        }

        error_code_t code = no_error;
        std::string message;
    };

    template <typename Tpayload>
    struct result {
    	result() = default;

    	static void maid(Tpayload pl) {}

    	result(Tpayload pl = { }, error e = { }) :
            payload(std::make_unique<Tpayload>(std::move(pl))),
            error(std::move(e))
        { }

    	result(error e) : error(std::move(e)) { }

    	result(result<Tpayload>&& other) noexcept : 
            payload(std::move(other.payload)),
            error(std::move(other.error))
        { }

        virtual ~result() = default;

        std::unique_ptr<Tpayload> payload;
        error error;
    };
}