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

    	result(Tval pl, error e = { }) :
            m_value(std::make_unique<Tval>(std::move(pl))),
            m_error(std::move(e))
        { }

    	result(error e) : m_error(std::move(e)) { }

    	result(result<Tval>&& other) noexcept :
            m_value(std::move(other.m_value)),
            m_error(std::move(other.m_error))
        { }

        virtual ~result() = default;

        [[nodiscard]] Tval& value() const { return *m_value; }

        [[nodiscard]] error& error() { return m_error; }

        explicit operator bool() const {
            return !m_error;
        }

    private:
        std::unique_ptr<Tval> m_value;
        bt::error m_error;
    };
}