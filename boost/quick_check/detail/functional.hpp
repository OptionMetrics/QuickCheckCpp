///////////////////////////////////////////////////////////////////////////////////////////////////
// \file functional.hpp
// \brief function object helpers
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_FUNCTIONAL_HPP_INCLUDED
#define QCHK_DETAIL_FUNCTIONAL_HPP_INCLUDED

#include <boost/utility/declval.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Fun>
        struct unary
        {
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
        struct binary
        {
            template<typename Sig>
            struct result
            {};

            template<typename This, typename T, typename U>
            struct result<This(T, U)>
            {
                typedef decltype(boost::declval<This>()(boost::declval<T>(), boost::declval<U>())) type;
            };
        };
    }
}


QCHK_BOOST_NAMESPACE_END

#endif
