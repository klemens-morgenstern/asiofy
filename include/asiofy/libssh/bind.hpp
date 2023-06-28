//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_BASIC_BIND_HPP
#define ASIOFY_LIBSSH_BASIC_BIND_HPP

#include <libssh/server.h>
#include <asiofy/libssh/detail/config.hpp>
#include <asiofy/libssh/detail/handles.hpp>
#include "error.hpp"
#include "basic_session.hpp"

#include <boost/asio/basic_socket_acceptor.hpp>

namespace asiofy
{
namespace libssh
{

// helpers to make this type-safe

template<ssh_bind_options_e Option, typename T>
struct bind_option
{
  constexpr static ssh_bind_options_e option = Option;
  using value_type = T;
  value_type value = {};
  bind_option() = default;
  explicit bind_option(value_type value) : value(value) {}

  bool apply(ssh_bind bind) const
  {
    return ssh_bind_options_set(bind, Option, &value) == SSH_OK;
  }
};

template<ssh_bind_options_e Option>
struct bind_option<Option, const char *>
{
  constexpr static ssh_bind_options_e option = Option;
  using value_type = const char *;
  value_type value = {};
  bind_option() = default;
  explicit bind_option(value_type value) : value(value) {}

  bool apply(ssh_bind bind) const
  {
    return ssh_bind_options_set(bind, Option, value) == SSH_OK;
  }
};


using bindaddr                  = bind_option<SSH_BIND_OPTIONS_BINDADDR,                  const char *>;
using bindport                  = bind_option<SSH_BIND_OPTIONS_BINDPORT,                  unsigned >;
using bindport_str              = bind_option<SSH_BIND_OPTIONS_BINDPORT_STR,              const char *>;
using hostkey                   = bind_option<SSH_BIND_OPTIONS_HOSTKEY,                   const char *>;
using dsakey                    = bind_option<SSH_BIND_OPTIONS_DSAKEY,                    const char *>;
using rsakey                    = bind_option<SSH_BIND_OPTIONS_RSAKEY,                    const char *>;
using banner                    = bind_option<SSH_BIND_OPTIONS_BANNER,                    const char *>;
using log_verbosity             = bind_option<SSH_BIND_OPTIONS_LOG_VERBOSITY,             int>;
using log_verbosity_str         = bind_option<SSH_BIND_OPTIONS_LOG_VERBOSITY_STR,         const char *>;
using ecdsakey                  = bind_option<SSH_BIND_OPTIONS_ECDSAKEY,                  const char *>;
using import_key                = bind_option<SSH_BIND_OPTIONS_IMPORT_KEY,                ssh_key>;
using key_exchange              = bind_option<SSH_BIND_OPTIONS_KEY_EXCHANGE,              const char *>;
using ciphers_c_s               = bind_option<SSH_BIND_OPTIONS_CIPHERS_C_S,               const char *>;
using ciphers_s_c               = bind_option<SSH_BIND_OPTIONS_CIPHERS_S_C,               const char *>;
using hmac_c_s                  = bind_option<SSH_BIND_OPTIONS_HMAC_C_S,                  const char *>;
using hmac_s_c                  = bind_option<SSH_BIND_OPTIONS_HMAC_S_C,                  const char *>;
using config_dir                = bind_option<SSH_BIND_OPTIONS_CONFIG_DIR,                const char *>;
using pubkey_accepted_key_types = bind_option<SSH_BIND_OPTIONS_PUBKEY_ACCEPTED_KEY_TYPES, const char *>;
using hostkey_algorithms        = bind_option<SSH_BIND_OPTIONS_HOSTKEY_ALGORITHMS,        const char *>;
using process_config            = bind_option<SSH_BIND_OPTIONS_PROCESS_CONFIG,            bool>;
using moduli                    = bind_option<SSH_BIND_OPTIONS_MODULI,                    const char *>;
using rsa_min_size              = bind_option<SSH_BIND_OPTIONS_RSA_MIN_SIZE,              int>;

template<ssh_bind_options_e Option, typename T>
bool apply_config(ssh_bind bind, const bind_option<Option, T> & bo)
{
  return bo.apply(bind);
}

template<ssh_bind_options_e Option, typename T>
bool apply_config(unique_handle<ssh_bind> bind, const bind_option<Option, T> & bo)
{
  return bo.apply(bind.get());
}

namespace detail
{

template<typename Protocol, typename Executor>
struct initiate_async_accept
{
  boost::asio::basic_socket_acceptor<Protocol, Executor> & acceptor;
  error_info * ei = nullptr;
  unique_handle<ssh_session> session;

  template<typename Self>
  void operator()(Self && self)
  {
    acceptor.async_accept(std::move(self));
  }
  template<typename Self>
  void operator()(Self && self, error_code ec,
                  net::basic_stream_socket<net::generic::stream_protocol, Executor> socket)
  {
    if (ec)
      return self.complete(ec, std::move(*session));
    session.reset(ssh_new());
    auto code = ssh_bind_accept_fd(session.get(), session.get(), socket.release(ec));
    if (code != SSH_OK)
    {
      ASIOFY_ASSIGN_EC(ec, code, ssh_category());
      if (ei != nullptr)
        ei->set_message(ssh_get_error(session.get()));
      session.reset();
    }
    return self.complete(ec, std::move(session));
  }
};
}

template<typename Protocol,
         typename Executor,
         BOOST_ASIO_COMPLETION_TOKEN_FOR(void(error_code, unique_handle<ssh_session>)) AcceptToken
           BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>
BOOST_ASIO_INITFN_RESULT_TYPE(AcceptToken, void(error_code, unique_handle<ssh_session>))
async_accept(
    boost::asio::basic_socket_acceptor<Protocol, Executor> & acceptor,
    error_info & ei,
    AcceptToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))
{
  return net::async_compose<AcceptToken, void(error_code, unique_handle<ssh_session>)>
  (
      detail::initiate_async_accept<Protocol, Executor>{acceptor, &ei}, token
  );
}

template<typename Protocol,
         typename Executor,
         BOOST_ASIO_COMPLETION_TOKEN_FOR(void(error_code, unique_handle<ssh_session>)) AcceptToken
           BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(Executor)>
BOOST_ASIO_INITFN_RESULT_TYPE(AcceptToken, void(error_code, unique_handle<ssh_session>))
async_accept(
    boost::asio::basic_socket_acceptor<Protocol, Executor> & acceptor,
    AcceptToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(Executor))
{
  return net::async_compose<AcceptToken, void(error_code, unique_handle<ssh_session>)>
  (
      detail::initiate_async_accept<Protocol, Executor>{acceptor}, token
  );
}





template<typename Executor = net::any_io_executor>
struct basic_bind
{
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  /// Rebinds the session type to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The pipe type when rebound to the specified executor.
    typedef basic_bind<Executor1> other;
  };

  /// The native representation of a session.
  typedef ssh_bind native_handle_type;
  using next_layer_type = net::basic_socket_acceptor<net::generic::stream_protocol, executor_type>;
        next_layer_type & next_layer()       { return acceptor_; }
  const next_layer_type & next_layer() const { return acceptor_; }


  explicit basic_bind(const executor_type& ex)
      : acceptor_(ex)
  {
  }

  template <typename ExecutionContext>
  explicit basic_bind(ExecutionContext& context,
                         typename std::enable_if<
                             std::is_convertible<ExecutionContext&, net::execution_context&>::value,
                             int>::type = 0)
      : acceptor_(context.get_executor())
  {
  }
  basic_bind(const executor_type& ex,
                const native_handle_type& native_Handle)
      : acceptor_(ex), handle_(native_Handle)
  {
  }

  template <typename ExecutionContext>
  basic_bind(ExecutionContext& context,
                const native_handle_type& native_handle,
                typename std::enable_if<
                    std::is_convertible<ExecutionContext&, net::execution_context&>::value,
                    int >::type = 0) : acceptor_(context.get_executor()), handle_(native_handle)
  {
  }

  basic_bind(basic_bind&& other)
      : acceptor_(other.acceptor_), handle_(std::move(other.handle_))
  {
  }

  basic_bind& operator=(basic_bind&& other)
  {
    acceptor_ = std::move(other.acceptor_);
    handle_ = std::move(other.handle_);
    return *this;
  }

  // All pipes have access to each other's implementations.
  template <typename Executor1>
  friend class basic_bind;

  template <typename Executor1>
  basic_bind(basic_bind<Executor1>&& other,
                typename std::enable_if<
                    std::is_convertible<Executor1, Executor>::value, int>::type = 0)
      : acceptor_(other.acceptor_), handle_(std::move(other.handle_))
  {
  }

  template <typename Executor1>
    typename std::enable_if<
        std::is_convertible<Executor1, Executor>::value,
        basic_bind&
    >::type operator=(basic_bind<Executor1>&& other)
  {
    acceptor_ = other.acceptor_;
    handle_ = std::move(other.handle_);
    return *this;
  }
  const executor_type& get_executor() BOOST_ASIO_NOEXCEPT
  {
    return acceptor_.get_executor();
  }

  void assign(native_handle_type native_handle)
  {
    handle_.reset(native_handle);
  }

  native_handle_type release()
  {
    return handle_.release();
  }

  native_handle_type native_handle()
  {
    return handle_.get();
  }

  bool options_set(enum ssh_bind_options_e type, const char *value)
  {
    return ssh_bind_options_set(handle_.get(), type, value) == SSH_OK;
  }
  bool options_set(enum ssh_bind_options_e type, unsigned int value)
  {
    return ssh_bind_options_set(handle_.get(), type, &value) == SSH_OK;
  }
  bool options_parse_config(const char * filename)
  {
    return ssh_bind_options_parse_config(handle_.get(), filename) == SSH_OK;
      ASIOFY_LIBSSH_THROW_ERROR(handle_.get());
  }

  void listen()
  {
    int res = ssh_bind_listen(handle_.get());
    if (res != SSH_OK)
      ASIOFY_LIBSSH_THROW_ERROR(handle_.get());
  }

  void listen(error_code & ec, error_info & ei)
  {
    int res = ssh_bind_listen(handle_.get());
    if (res != SSH_OK)
      ASIOFY_LIBSSH_ASSIGN_ERROR(ec, ei, handle_.get());
  }


  basic_session<executor_type> accept()
  {
    basic_session<executor_type> sess{get_executor()};
    ssh_bind_set_blocking(handle_.get(), 1);
    int res = ssh_bind_accept(handle_.get(), sess.native_handle());
    if (res != SSH_OK)
      ASIOFY_LIBSSH_THROW_ERROR(handle_.get());

    return sess;
  }

  basic_session<executor_type> accept(error_code & ec, error_info & ei)
  {
    basic_session<executor_type> sess{get_executor()};
    ssh_bind_set_blocking(handle_.get(), 1);
    int res = ssh_bind_accept(handle_.get(), sess.native_handle());
    if (res != SSH_OK)
      ASIOFY_LIBSSH_ASSIGN_ERROR(ec, ei, handle_.get());
    return sess;
  }

    template <BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, basic_session<executor_type>)) ConnectToken
    BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
      BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void (error_code, basic_session<executor_type>))
  async_accept(ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
  {
    return net::async_compose<ConnectToken, void (error_code, basic_session<executor_type>)>
        (
            initiate_async_accept{this}, token, acceptor_
        );
  }


  template <BOOST_ASIO_COMPLETION_TOKEN_FOR(void(error_code, basic_session<executor_type>)) ConnectToken
    BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
      BOOST_ASIO_INITFN_RESULT_TYPE(ConnectToken, void(error_code, basic_session<executor_type>))
  async_accept(
      error_info & ei,
      ConnectToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
  {
    return net::async_compose<ConnectToken, void(error_code, basic_session<executor_type>)>
        (
            initiate_async_accept{this, &ei}, token, acceptor_
        );
  }

  ~basic_bind()
  {
    error_code ec;
    acceptor_.release(ec);
  }

 private:
  struct initiate_async_accept
  {
      basic_bind * this_;
      error_info * ei = nullptr;
      std::shared_ptr<basic_session<executor_type>> session;

      template<typename Self>
      void operator()(Self && self)
      {
        ssh_bind_set_blocking(this_->handle_.get(), 0);
        session = std::allocate_shared<basic_session<executor_type>>(self.get_allocator(), this_->get_executor());
        this_->acceptor_.template async_accept(net::socket_base::wait_read, std::move(self));
      }
      template<typename Self>
      void operator()(Self && self, error_code ec,
                      net::basic_stream_socket<net::generic::stream_protocol, executor_type> socket)
      {
        if (ec)
          return self.complete(ec, std::move(*session));

        ssh_bind_accept_fd(this_->native_handle(), session->native_handle(), socket.release(ec));
        return self.complete(ec, std::move(*session));
      }
  };

  net::basic_socket_acceptor<net::generic::stream_protocol, executor_type> acceptor_;
  detail::unique_handle<ssh_bind> handle_{ssh_bind_new()};
};

}
}


#endif //ASIOFY_LIBSSH_BASIC_BIND_HPP
