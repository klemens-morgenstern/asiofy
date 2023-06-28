//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_WRAPPER_HPP
#define ASIOFY_LIBSSH_WRAPPER_HPP

#include <libssh/libssh.h>
#include <asiofy/libssh/error.hpp>
#include <asiofy/libssh/detail/config.hpp>

#include <boost/asio/compose.hpp>
#include <boost/asio/socket_base.hpp>

namespace asiofy
{
namespace libssh
{

template<typename Executor>
struct basic_session;

namespace detail
{

template<typename Executor,
         typename Func,
         net::socket_base::wait_type WaitType = net::socket_base::wait_read>
struct async_session_op_t
{
  basic_session<Executor> & sess;
  Func func;
  error_info * ei;
  template<typename Self>
  void operator()(Self && self)
  {
    ssh_set_blocking(sess.native_handle(), 0);
    sess.next_layer().async_wait(WaitType, std::move(self));
  }

  template<typename Self>
  void operator()(Self && self, error_code ec)
  {
    if (ec)
      return self.complete(ec);
    int res = func(sess.native_handle());
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

template<net::socket_base::wait_type WaitType = net::socket_base::wait_read ,
         typename Executor, typename Func,
         ASIOFY_COMPLETION_TOKEN_FOR(void (error_code)) CompletionToken ASIOFY_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>
ASIOFY_INITFN_RESULT_TYPE(CompletionToken, void (error_code))
async_session_op(basic_session<Executor> & sess, Func && func,
                 error_info * ei, CompletionToken && token ASIOFY_DEFAULT_COMPLETION_TOKEN(Executor))
{
  return net::async_compose<CompletionToken, void (error_code)>
      (
          detail::async_session_op_t<Executor, typename std::decay<Func>::type, WaitType>{
            sess, std::forward<Func>(func), ei}, token, sess
      );
}


template<typename Func, typename ... Args>
auto bind_back(Func && func, Args ... args)
{
  return std::bind(std::forward<Func>(func), std::placeholders::_1, std::move(args)...);
}

}
}
}

#endif //ASIOFY_LIBSSH_WRAPPER_HPP
