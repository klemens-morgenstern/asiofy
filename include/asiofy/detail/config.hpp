//
// Copyright (c) 2022 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIOFY_CONFIG_HPP
#define ASIOFY_CONFIG_HPP

#if defined(ASIOFY_STANDALONE)
namespace asio {}
#include <system_error>
#else
namespace boost { namespace asio {} }
#include <boost/system/system_error.hpp>
#endif

namespace asiofy
{

#if defined(ASIOFY_STANDALONE)
using std::error_code;
using std::system_error;
using std::error_category;
using std::system_error;

namespace net = asio;
#define ASIOFY_ASSIGN_EC(Name, ...) Name.assign(__VA_ARGS__);

template <typename Exception>
inline void throw_exception(const Exception& e)
{
  throw e;
}


#else
namespace net = boost::asio;
using boost::system::error_code;
using boost::system::system_error;
using boost::system::error_category;
using boost::system::system_error;
using boost::throw_exception;

#define ASIOFY_ASSIGN_EC(Name, ...) \
  {                                 \
    constexpr static boost::source_location loc = BOOST_CURRENT_LOCATION; \
    Name.assign(__VA_ARGS__, &loc); \
  }
using boost::throw_exception;

#endif

}

#endif //ASIOFY_CONFIG_HPP
