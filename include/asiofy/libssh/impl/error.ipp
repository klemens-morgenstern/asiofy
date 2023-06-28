//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_IMPL_ERROR_IPP
#define ASIOFY_LIBSSH_IMPL_ERROR_IPP

#include <libssh/libssh.h>
#include <asiofy/libssh/error.hpp>
#include <boost/core/detail/string_view.hpp>
#include <algorithm>

namespace asiofy
{
namespace libssh
{

struct ssh_category_t final : error_category
{
  ssh_category_t() : error_category(0x7d4c7b49e8a3edull) {}

  std::string message( int ev ) const override
  {
    switch (ev)
    {
      case SSH_NO_ERROR:       return "no error";
      case SSH_REQUEST_DENIED: return "request denied";
      case SSH_FATAL:          return "fatal";
      case SSH_EINTR:          return "interrupted";
      default:
        return "unknown error";
    }
  }

  const char * name() const BOOST_NOEXCEPT override
  {
    return "libssh";
  }
};

error_category & ssh_category()
{
  static ssh_category_t cat;
  return cat;
}


}
}


#endif //ASIOFY_LIBSSH_IMPL_ERROR_IPP
