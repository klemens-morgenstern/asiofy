//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_ERROR_HPP
#define ASIOFY_LIBSSH_ERROR_HPP

#include <boost/system/error_category.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace asiofy
{
namespace libssh
{

/// The type of error code used by the library \ingroup reference
using error_code = boost::system::error_code;

/// The type of system error thrown by the library \ingroup reference
using system_error = boost::system::system_error;

/// The type of error category used by the library \ingroup reference
using error_category = boost::system::error_category;


error_category & ssh_category();


// copy-pastaed from anarthal/mysql --> alias ?

/**
 * \brief Additional information about error conditions
 * \ingroup reference
 * \details Contains an error message describing what happened. Not all error
 * conditions are able to generate this extended information - those that
 * can't have an empty error message.
 */
class error_info
{
    std::string msg_;

  public:
    /// Default constructor.
    error_info() = default;

    /// Initialization constructor.
    error_info(std::string&& err) noexcept : msg_(std::move(err)) {}

    /// Gets the error message.
    const std::string& message() const noexcept { return msg_; }

    /// Sets the error message.
    void set_message(std::string&& err) { msg_ = std::move(err); }

    /// Restores the object to its initial state.
    void clear() noexcept { msg_.clear(); }
};

}
}


#if defined(BOOST_SQLITE_HEADER_ONLY)
#include <asiofy/libssh/impl/error.ipp>
#endif


#endif //ASIOFY_LIBSSH_ERROR_HPP
