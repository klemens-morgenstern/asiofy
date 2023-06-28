//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_DETAIL_CONFIG_HPP
#define ASIOFY_LIBSSH_DETAIL_CONFIG_HPP

#include <asiofy/detail/config.hpp>

#define ASIOFY_LIBSSH_THROWING_OVERLOAD(...)

#define ASIOFY_LIBSSH_ASSIGN_ERROR(Ec, Ei, Handle)                    \
  {                                                                   \
    ASIOFY_ASSIGN_EC(Ec, ssh_get_error_code(Handle), ssh_category()); \
    Ei.set_message(ssh_get_error(Handle));                            \
  }

#define ASIOFY_LIBSSH_THROW_ERROR(Handle)                             \
  {                                                                   \
    error_code ec;                                                    \
    ASIOFY_ASSIGN_EC(ec, ssh_get_error_code(Handle), ssh_category()); \
    throw_exception(system_error(ec, ssh_get_error(Handle)));         \
  }

#define ASIOFY_LIBSSH_THROW_ON_ERROR(Call, Handle)                    \
  if (Call != SSH_OK)                                                 \
  {                                                                   \
    error_code ec;                                                    \
    ASIOFY_ASSIGN_EC(ec, ssh_get_error_code(Handle), ssh_category()); \
    throw_exception(system_error(ec, ssh_get_error(Handle)));         \
  }

#define ASIOFY_COMPLETION_TOKEN_FOR(...)          BOOST_ASIO_COMPLETION_TOKEN_FOR(__VA_ARGS__)
#define ASIOFY_DEFAULT_COMPLETION_TOKEN_TYPE(...) BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(__VA_ARGS__)
#define ASIOFY_INITFN_RESULT_TYPE(...)            BOOST_ASIO_INITFN_RESULT_TYPE(__VA_ARGS__)
#define ASIOFY_DEFAULT_COMPLETION_TOKEN(...)      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(__VA_ARGS__)


#endif //ASIOFY_LIBSSH_DETAIL_CONFIG_HPP
