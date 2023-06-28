//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_LIBSSH_HANDLES_HPP
#define ASIOFY_LIBSSH_HANDLES_HPP

#include <libssh/libssh.h>
#include <memory>

namespace asiofy
{
namespace libssh
{
namespace detail
{


template<typename Handle, void (*Free)(Handle)>
struct handle_deleter
{
  void operator()(Handle h) { Free(h); }
};

template<typename Handle, void (*Free)(Handle)>
using unique_handle =
    std::unique_ptr<
        typename std::remove_pointer<Handle>::type,
        handle_deleter<Handle, Free>
    >;

}
}
}

#endif //ASIOFY_LIBSSH_HANDLES_HPP
