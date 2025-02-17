// https://github.com/kunitoki/LuaBridge3
// Copyright 2022, Lucio Asnaghi
// SPDX-License-Identifier: MIT

#pragma once

#include "Errors.h"

namespace luabridge {

//=================================================================================================
/**
 * @brief Simple result class containing a result.
 */
struct Result
{
    Result() = default;

    Result(std::error_code ec) noexcept
        : m_ec(ec)
        , m_nargs(1)
    {
    }

    Result(const Result&) = default;
    Result(Result&&) = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) = default;

    explicit operator bool() const noexcept
    {
        return !m_ec;
    }

    std::error_code error() const noexcept
    {
        return m_ec;
    }

    operator std::error_code() const noexcept
    {
        return m_ec;
    }

    std::string message() const
    {
        return m_ec.message();
    }

    inline int num_args() const
    {
        return m_nargs;
	}

	inline void num_args(int nargs)
	{
        m_nargs = nargs;
	}

private:
    std::error_code m_ec;
    int m_nargs = 1;
};

//=================================================================================================
/**
 * @brief Simple result class containing or a type T or an error code.
 */
template <class T>
struct TypeResult
{
    TypeResult() = default;

    template <class U, class = std::enable_if_t<std::is_convertible_v<U, T> && !std::is_same_v<std::decay_t<U>, std::error_code>>>
    TypeResult(U&& value) noexcept
        : m_value(std::in_place, std::forward<U>(value))
    {
    }

    TypeResult(std::error_code ec) noexcept
        : m_value(makeUnexpected(ec))
    {
    }

    TypeResult(const TypeResult&) = default;
    TypeResult(TypeResult&&) = default;
    TypeResult& operator=(const TypeResult&) = default;
    TypeResult& operator=(TypeResult&&) = default;

    explicit operator bool() const
    {
        return m_value.hasValue();
    }

    const T& value() const
    {
        return m_value.value();
    }

    T& operator*() &
    {
        return m_value.value();
    }

    T operator*() &&
    {
        return std::move(m_value.value());
    }

    const T& operator*() const&
    {
        return m_value.value();
    }

    std::error_code error() const
    {
        return m_value.error();
    }

    operator std::error_code() const
    {
        return m_value.error();
    }

    std::string message() const
    {
        return m_value.error().message();
    }

private:
    Expected<T, std::error_code> m_value;
};

template <class U>
inline bool operator==(const TypeResult<U>& lhs, const U& rhs) noexcept
{
    return lhs ? *lhs == rhs : false;
}

template <class U>
inline bool operator==(const U& lhs, const TypeResult<U>& rhs) noexcept
{
    return rhs == lhs;
}

template <class U>
inline bool operator!=(const TypeResult<U>& lhs, const U& rhs) noexcept
{
    return !(lhs == rhs);
}

template <class U>
inline bool operator!=(const U& lhs, const TypeResult<U>& rhs) noexcept
{
    return !(rhs == lhs);
}

} // namespace luabridge
