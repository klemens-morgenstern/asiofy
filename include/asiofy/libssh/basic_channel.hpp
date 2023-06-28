//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_BASIC_CHANNEL_HPP
#define ASIOFY_BASIC_CHANNEL_HPP

#include <boost/asio/any_io_executor.hpp>
#include <asiofy/libssh/detail/config.hpp>

namespace asiofy
{
namespace libssh
{

template<typename Executor = net::any_io_executor>
struct basic_channel
{
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  void open_session();
  void open_session(error_code & ec, error_info & ei);
  template<
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
      BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
        BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_open_session(
      RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));


  void open_x11(const char * orig_addr, int orig_port);
  void open_x11(const char * orig_addr, int orig_port, error_code & ec, error_info & ei);
  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
    async_open_x11(const char * orig_addr, int orig_port,
        RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));


  void poll(bool is_stderr);
  void poll_timeout(int timeout, bool is_stderr);

  template<typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence & buffers, bool istderr);

  template<typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence & buffers, bool istderr, error_code & ec);

  template<typename MutableBufferSequence,
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, std::size_t)) ReadToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(ReadToken, void (error_code, std::size_t))
  async_read_some(
      const MutableBufferSequence & buffers, bool istderr,
      ReadToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
  template<typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence & buffers, bool istderr);

  template<typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence & buffers, bool istderr, error_code & ec);

  template<typename ConstBufferSequence,
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, std::size_t)) ReadToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(ReadToken, void (error_code, std::size_t))
  async_write_some(
      const ConstBufferSequence & buffers, bool istderr,
      ReadToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
  struct stdreader
  {
    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence & buffers)
    {
      return self->read_some(buffers, istderr);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence & buffers, error_code & ec)
    {
      return self->read_some(buffers, istderr, ec);
    }

    template<typename MutableBufferSequence,
        BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, std::size_t)) ReadToken
          BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
            BOOST_ASIO_INITFN_RESULT_TYPE(ReadToken, void (error_code, std::size_t))
    async_read_some(
        const MutableBufferSequence & buffers,
        ReadToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
      return self->async_read_some(buffers, istderr, std::forward<ReadToken>(token));
    }
    
    template<typename ConstBufferSequence>
    std::size_t write_some(const ConstBufferSequence & buffers)
    {
      return self->write_some(buffers, istderr);
    }
  
    template<typename ConstBufferSequence>
    std::size_t write_some(const ConstBufferSequence & buffers, error_code & ec)
    {
      return self->write_some(buffers, istderr, ec);
    }
  
    template<typename ConstBufferSequence,
        BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, std::size_t)) WriteToken
          BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
            BOOST_ASIO_INITFN_RESULT_TYPE(WriteToken, void (error_code, std::size_t))
    async_write_some(
        const ConstBufferSequence & buffers,
        WriteToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
      return self->async_write_some(buffers, istderr, std::forward<WriteToken>(token));

    }

    friend struct basic_channel;
   private:
    basic_channel * self;
    bool istderr;
  };

  stdreader get_stdout() {return stdreader{this, false};}
  stdreader get_stderr() {return stdreader{this,  true};}

  // -> callback ssh_channel_open_request_auth_agent_callback
  void request_auth_agent();
  void request_auth_agent(error_code & ec, error_info & ei);
  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_auth_agent(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));


  void request_env(const char * name, const char * value);
  void request_env(const char * name, const char * value, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_env(const char * name, const char * value,
                    RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_exec(const char * cmd);
  void request_exec(const char * cmd, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_exec(const char * cmd,
                    RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_pty();
  void request_pty(error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_pty(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_pty_size(const char * terminal, int col, int row);
  void request_pty_size(const char * terminal, int col, int row, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_pty_size(const char * terminal, int col, int row,
                         RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_send_break(std::uint32_t length);
  void request_send_break(std::uint32_t length, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_send_break(std::uint32_t length,
                           RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
  void request_send_exit_signal(const char *sig, int core, const char * errmsg, const char * lang);
  void request_send_exit_signal(const char *sig, int core, const char * errmsg, const char * lang, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_send_exit_signal(const char *sig, int core, const char * errmsg, const char * lang,
                                 RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
  void request_send_exit_status(int exit_status);
  void request_send_exit_status(int exit_status, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_send_exit_status(int exit_status,
                                 RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
    
  void request_send_signal(const char *sig);
  void request_send_signal(const char *sig, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_send_signal(const char *sig,
                            RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  

  void request_sftp();
  void request_sftp(error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_sftp(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_shell();
  void request_shell(error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_shell(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void request_subsystem(const char * subsys);
  void request_subsystem(const char * subsys, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_subsystem(const char * subsys,
                          RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));
  
  void request_x11(int single_connection, const char * protocol, const char * cookie, int screen_number);
  void request_x11(int single_connection, const char * protocol, const char * cookie, int screen_number, error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_request_x11(int single_connection, const char * protocol, const char * cookie, int screen_number,
                    RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  void send_eof();
  void send_eof(error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code))
  async_send_eof(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));

  std::uint32_t window_size();
  std::uint32_t window_size(error_code & ec, error_info & ei);

  template<
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (error_code, std::uint32_t)) RequestToken
        BOOST_ASIO_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
          BOOST_ASIO_INITFN_RESULT_TYPE(RequestToken, void (error_code, std::uint32_t))
  async_window_size(RequestToken && token BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type));


 private:
  detail::unique_handle<ssh_channel, ssh_channel_free> handle_{};

};

}
}

#endif //ASIOFY_BASIC_CHANNEL_HPP
