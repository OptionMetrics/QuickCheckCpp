///////////////////////////////////////////////////////////////////////////////////////////////////
// \file functional.hpp
// \brief function object helpers
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_FUNCTIONAL_HPP_INCLUDED
#define QCHK_DETAIL_FUNCTIONAL_HPP_INCLUDED

#include <boost/config.hpp>
#include <boost/utility/declval.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct empty
        {};

        template<typename Base = empty>
        struct unary
          : Base
        {
            explicit BOOST_CONSTEXPR unary(Base const &base = Base())
              : Base(base)
            {}

            template<typename Sig>
            struct result
            {};

            template<typename This, typename T>
            struct result<This(T)>
            {
                typedef decltype(boost::declval<This>()(boost::declval<T>())) type;
            };
        };

        template<typename Fun>
        BOOST_CONSTEXPR unary<Fun> make_unary(Fun const &fun)
        {
            return unary<Fun>(fun);
        }

        template<typename Base = empty>
        struct binary
          : Base
        {
            explicit BOOST_CONSTEXPR binary(Base const &base = Base())
              : Base(base)
            {}

            template<typename Sig>
            struct result
            {};

            template<typename This, typename T, typename U>
            struct result<This(T, U)>
            {
                typedef decltype(boost::declval<This>()(boost::declval<T>(), boost::declval<U>())) type;
            };
        };

        template<typename Fun>
        BOOST_CONSTEXPR binary<Fun> make_binary(Fun const &fun)
        {
            return binary<Fun>(fun);
        }
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
