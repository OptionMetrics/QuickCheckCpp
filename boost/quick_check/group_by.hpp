///////////////////////////////////////////////////////////////////////////////////////////////////
// \file classify.hpp
// \brief Definition of the classify grammar element
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GROUP_BY_HPP_INCLUDED
#define QCHK_GROUP_BY_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/quick_check/detail/grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct group_by_
        {};
    }

    template<typename Expr>
    typename proto::result_of::make_expr<
        detail::group_by_
      , detail::quick_check_domain
      , phoenix::actor<Expr>
    >::type const
    group_by(phoenix::actor<Expr> const &e)
    {
        return proto::make_expr<detail::group_by_, detail::quick_check_domain>(e);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
