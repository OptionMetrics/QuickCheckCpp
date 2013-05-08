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
#include <boost/phoenix/core/domain.hpp>
#include <boost/phoenix/core/detail/function_eval.hpp>
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
        phoenix::detail::tag::function_eval
      , phoenix::phoenix_domain
      , detail::group_by_
      , Expr
    >::type const
    group_by(Expr const &e)
    {
        return proto::make_expr<phoenix::detail::tag::function_eval, phoenix::phoenix_domain>(
            detail::group_by_()
          , e
        );
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
