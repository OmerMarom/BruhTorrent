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

    template <typename Tval>
    class result {
    public:
    	result() = default;

    	result(Tval&& value) : m_value_or_error(std::forward<Tval>(value)) { }

    	result(error err) : m_value_or_error(std::move(err)) { }

    	result(result<Tval>&& other) noexcept :
            m_value_or_error(std::move(other.m_value_or_error))
        { }

        virtual ~result() = default;

        [[nodiscard]] Tval& value() { return std::get<Tval>(m_value_or_error); }

        [[nodiscard]] error& error() { return std::get<bt::error>(m_value_or_error); }

        Tval& operator*() { return value(); }

        explicit operator bool() const {
            return std::holds_alternative<Tval>(m_value_or_error);
        }

    private:
        std::variant<Tval, bt::error> m_value_or_error;
    };
}