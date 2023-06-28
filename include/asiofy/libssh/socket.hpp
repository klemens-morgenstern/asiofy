//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_WAIT_SOCKET_HPP
#define ASIOFY_WAIT_SOCKET_HPP

#include <asiofy/libssh/detail/config.hpp>
#include <boost/asio/basic_socket.hpp>

namespace asiofy
{
namespace libssh
{

template<typename Socket>
struct wait_socket : boost::asio::basic_socket<
{

};

}
}

#endif //ASIOFY_WAIT_SOCKET_HPP
