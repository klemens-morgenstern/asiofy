//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_DETAIL_MACROS_HPP
#define ASIOFY_LIBSSH_DETAIL_MACROS_HPP

#include <asiofy/libssh/detail/config.hpp>
#include <asiofy/libssh/error.hpp>

#include <boost/asio/socket_base.hpp>
#include <libssh/libssh.h>

namespace asiofy
{
namespace libssh
{

template<typename Executor>
struct basic_session;

namespace detail
{

template<typename Executor,
         int (*Func)(ssh_session),
         net::socket_base::wait_type WaitType>
struct async_wrap_session_async_call_0
{
  basic_session<Executor> & sess;
  error_info * ei;
  template<typename Self>
  void operator()(Self && self)
  {
    ssh_set_blocking(sess.native_handle(), 0);
    sess.next_layer().async_wait(net::socket_base::wait_read, std::move(self));
  }

  template<typename Self>
  void operator()(Self && self, error_code ec)
  {
    if (ec)
      return self.complete(ec);
    int res = Func(sess.native_handle());
    switch(res)
    {
      case SSH_OK:
        return self.complete(ec);
      case SSH_ERROR:
      {
        if (ei)
          ei->set_message(ssh_get_error(sess.native_handle()));
        constexpr static boost::source_location loc = BOOST_CURRENT_LOCATION;
        return self.complete(error_code(ssh_get_error_code(sess.native_handle()), ssh_category(), &loc));
      }
      case SSH_AGAIN:
        return sess.next_layer().async_wait(WaitType, std::move(self));
    }
  }
};

template<typename Executor,
    typename Arg0,
    int (*Func)(ssh_session, Arg0),
    net::socket_base::wait_type WaitType>
struct async_wrap_session_async_call_1
{
  basic_session<Executor> & sess;
  Arg0 arg0;
  error_info * ei;
  template<typename Self>
  void operator()(Self && self)
  {
    ssh_set_blocking(sess.native_handle(), 0);
    sess.next_layer().async_wait(net::socket_base::wait_read, std::move(self));
  }

  template<typename Self>
  void operator()(Self && self, error_code ec)
  {
    if (ec)
      return self.complete(ec);
    int res = Func(sess.native_handle(), arg0);
    switch(res)
    {
      case SSH_OK:
        return self.complete(ec);
      case SSH_ERROR:
      {
        if (ei)
          ei->set_message(ssh_get_error(sess.native_handle()));
        constexpr static boost::source_location loc = BOOST_CURRENT_LOCATION;
        return self.complete(error_code(ssh_get_error_code(sess.native_handle()), ssh_category(), &loc));
      }
      case SSH_AGAIN:
        return sess.next_layer().async_wait(WaitType, std::move(self));
    }
  }
};


}
}
}

#define ASIOFY_LIBSSH_WRAP_FREE_SESSION_ASYNC_CALL_0(WaitType, Name)                                                  \
template<typename Executor>                                                                                           \
void Name(basic_session<Executor> & sess, error_code & ec, error_info & ei)                                           \
{                                                                                                                     \
  ssh_set_blocking(sess.native_handle(), 1);                                                                          \
  if (ssh_##Name(sess.native_handle()) != SSH_OK)                                                                     \
    ASIOFY_LIBSSH_ASSIGN_ERROR(ec, ei, sess.native_handle())                                                          \
}                                                                                                                     \
                                                                                                                      \
template<typename Executor>                                                                                           \
void Name(basic_session<Executor> & sess)                                                                             \
{                                                                                                                     \
  ssh_set_blocking(sess.native_handle(), 1);                                                                          \
  if (ssh_##Name(sess.native_handle()) != SSH_OK)                                                                     \
    ASIOFY_LIBSSH_THROW_ERROR(sess.native_handle())                                                                   \
}                                                                                                                     \
                                                                                                                      \
template <typename Executor,                                                                                          \
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) ConnectToken                                                   \
      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>                                                             \
        BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void (error_code))                                                \
async_handle_key_exchange(basic_session<Executor> & sess,                                                             \
                          ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))                        \
{                                                                                                                     \
  return net::async_compose<ConnectToken, void (error_code)>                                                          \
      (                                                                                                               \
          ::asiofy::libssh::detail::async_wrap_session_async_call_0<                                                  \
                  Executor, &ssh_##Name,                                                                              \
                  net::socket_base::WaitType>{sess},                                                                  \
          token, sess                                                                                                 \
      );                                                                                                              \
}                                                                                                                     \
                                                                                                                      \
template <typename Executor,                                                                                          \
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) ConnectToken                                                   \
      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>                                                             \
        BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void (error_code))                                                \
async_handle_key_exchange(basic_session<Executor> & sess, error_info & ei,                                            \
                          ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))                        \
{                                                                                                                     \
  return net::async_compose<ConnectToken, void (error_code)>                                                          \
      (                                                                                                               \
          ::asiofy::libssh::detail::async_wrap_session_async_call_0<                                                  \
                  Executor, &ssh_##Name,                                                                              \
                  net::socket_base::WaitType>{sess, &ei},                                                             \
          token, sess                                                                                                 \
      );                                                                                                              \
}                                                                                                                     \


#define ASIOFY_LIBSSH_WRAP_FREE_SESSION_ASYNC_CALL_1(WaitType, Name, Arg0, ArgName)                                   \
template<typename Executor>                                                                                           \
void Name(basic_session<Executor> & sess, Arg0 ArgName, error_code & ec, error_info & ei)                             \
{                                                                                                                     \
  ssh_set_blocking(sess.native_handle(), 1);                                                                          \
  if (ssh_##Name(sess.native_handle(), ArgName) != SSH_OK)                                                            \
    ASIOFY_LIBSSH_ASSIGN_ERROR(ec, ei, sess.native_handle())                                                          \
}                                                                                                                     \
                                                                                                                      \
template<typename Executor>                                                                                           \
void Name(basic_session<Executor> & sess, Arg0 ArgName)                                                               \
{                                                                                                                     \
  ssh_set_blocking(sess.native_handle(), 1);                                                                          \
  if (ssh_##Name(sess.native_handle(), ArgName) != SSH_OK)                                                            \
    ASIOFY_LIBSSH_THROW_ERROR(sess.native_handle())                                                                   \
}                                                                                                                     \
                                                                                                                      \
template <typename Executor,                                                                                          \
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) ConnectToken                                                   \
      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>                                                             \
        BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void (error_code))                                                \
async_handle_key_exchange(basic_session<Executor> & sess, Arg0 ArgName,                                               \
                          ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))                        \
{                                                                                                                     \
  return net::async_compose<ConnectToken, void (error_code)>                                                          \
      (                                                                                                               \
          ::asiofy::libssh::detail::async_wrap_session_async_call_1<                                                  \
                  Executor, &ssh_##Name,                                                                              \
                  net::socket_base::WaitType>{sess, ArgName},                                                         \
          token, sess                                                                                                 \
      );                                                                                                              \
}                                                                                                                     \
                                                                                                                      \
template <typename Executor,                                                                                          \
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) ConnectToken                                                   \
      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>                                                             \
        BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void (error_code))                                                \
async_handle_key_exchange(basic_session<Executor> & sess, Arg0 ArgName, error_info & ei,                              \
                          ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))                        \
{                                                                                                                     \
  return net::async_compose<ConnectToken, void (error_code)>                                                          \
      (                                                                                                               \
          ::asiofy::libssh::detail::async_wrap_session_async_call_1<                                                  \
                  Executor, &ssh_##Name,                                                                              \
                  net::socket_base::WaitType>{sess, ArgName &ei},                                                     \
          token, sess                                                                                                 \
      );                                                                                                              \
}


#endif //ASIOFY_LIBSSH_DETAIL_MACROS_HPP
