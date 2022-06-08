#pragma once

#include "asserts.h"

namespace bt {
    using error_code_t = std::uint8_t;

    struct error {
        error(const error_code_t err_code = general_error, std::string msg = {}) :
            code(err_code),
            message(std::move(msg))
        { }

        explicit error(std::string msg) :
            code(general_error),
            message(std::move(msg))
        { }

    	error(const error& other) = default;
        error& operator=(const error& other) = default;

    	error(error&& other) noexcept :
            code(other.code),
            message(std::move(other.message))
        { }

        static constexpr error_code_t no_error = 0;
        static constexpr error_code_t general_error = 1;

        explicit operator bool() const { return code != no_error; }

        error_code_t code = no_error;
        std::string message;
    };

    template <typename Tval>
    class result {
    public:
    	result(const Tval& val) : m_value_or_error(val) { }
    	result(Tval&& val) : m_value_or_error(std::forward<Tval>(val)) { }

    	result(const error& err) : m_value_or_error(err) { }
    	result(error&& err) : m_value_or_error(std::forward<bt::error>(err)) { }

    	result(result<Tval>&& other) noexcept :
            m_value_or_error(std::move(other.m_value_or_error))
        { }

        Tval& operator*() { return value(); }
        const Tval& operator*() const { return value(); }
        Tval* operator->() { return &value(); }
        const Tval* operator->() const { return &value(); }

        explicit operator bool() const { return has_value(); }

        [[nodiscard]] Tval& value() {
            BT_ASSERT(has_value(), "Called result::value() on a error result.");
	        return std::get<Tval>(m_value_or_error);
        }

        [[nodiscard]] const Tval& value() const {
            BT_ASSERT(has_value(), "Called result::value() on a error result.");
            return std::get<Tval>(m_value_or_error);
        }

        [[nodiscard]] error& error() {
            BT_ASSERT(has_error(), "Called result::error() on a value result.");
	        return std::get<bt::error>(m_value_or_error);
        }

        [[nodiscard]] const bt::error& error() const {
            BT_ASSERT(has_error(), "Called result::error() on a value result.");
            return std::get<bt::error>(m_value_or_error);
        }

        [[nodiscard]] bool has_value() const {
            return std::holds_alternative<Tval>(m_value_or_error);
        }

        [[nodiscard]] bool has_error() const {
            return std::holds_alternative<bt::error>(m_value_or_error);
        }

    private:
        std::variant<Tval, bt::error> m_value_or_error;
    };
}