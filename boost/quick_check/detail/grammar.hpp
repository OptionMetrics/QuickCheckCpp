///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file grammar.hpp
/// \brief Definition of the quick_check EDSL's grammar
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_GRAMMAR_HPP_INCLUDED
#define QCHK_DETAIL_GRAMMAR_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/proto/proto.hpp>
#include <boost/phoenix/core/meta_grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        // group_by(_1 % 5)
        struct Grouped
          : proto::nary_expr<
                detail::group_by_
              , phoenix::meta_grammar
            >
        {};

        // classify(_1>_2, "greater")
        struct Classify
          : proto::nary_expr<
                detail::classify_
              , phoenix::meta_grammar
              , proto::terminal<std::string>
            >
        {};

        // classify(_1>_2, "greater") | classify(_1<_2, "less") | classify(_1==_2, "equal")
        struct Classifiers
          : proto::or_<
                proto::bitwise_or<Classifiers, Classify>
              , Classify
            >
        {};

        // group_by(_1%2) | classify(_1>_2, "greater") | classify(_1<_2, "less") | classify(_1==_2, "equal")
        struct GroupedClassifiers
          : proto::or_<
                proto::bitwise_or<Grouped, Classify>
              , proto::bitwise_or<GroupedClassifiers, Classify>
            >
        {};

        // All valid combinations of combinators
        struct Combinators
          : proto::or_<
                Grouped
              , Classifiers
              , GroupedClassifiers
            >
        {};

        // Combinators with an expression, or just an expression
        struct CombinatorExpr
          : proto::or_<
                proto::bitwise_or<Combinators, phoenix::meta_grammar>
              , phoenix::meta_grammar
            >
        {};

        // A quick-check expr that has a conditional test
        struct ConditionalExpr
          : proto::shift_right_assign<phoenix::meta_grammar, CombinatorExpr>
        {};

        // A QuickCheckExpr is one of:
        //      ordered(_1) >>=
        //          group_by(_1 % 5) |
        //              classify(_1>_2, "greater") | classify(_1<_2, "less") | classify(_1==_2, "equal") |
        //                  _1 + _2 == _2 + _1
        // or
        //      group_by(_1 % 5) |
        //          classify(_1>_2, "greater") | classify(_1<_2, "less") | classify(_1==_2, "equal") |
        //              _1 + _2 == _2 + _1
        // or
        //      classify(_1>_2, "greater") | classify(_1<_2, "less") | classify(_1==_2, "equal") |
        //          _1 + _2 == _2 + _1
        // or
        //      _1 + _2 == _2 + _1
        struct QuickCheckExpr
          : proto::or_<
                ConditionalExpr
              , CombinatorExpr
            >
        {};

        template<typename Expr>
        struct quick_check_expr;

        struct quick_check_domain
          : proto::domain<proto::pod_generator<quick_check_expr>, Combinators>
        {
            template<typename T>
            struct as_child
              : as_expr<T>
            {};
        };

        template<typename Expr>
        struct quick_check_expr
        {
            BOOST_PROTO_BASIC_EXTENDS(Expr, quick_check_expr<Expr>, quick_check_domain)
        };

        // Ugh, this is SO distasteful
        template<typename L, typename R>
        typename proto::result_of::make_expr<
            proto::tag::bitwise_or
          , quick_check_domain
          , quick_check_expr<L>
          , phoenix::actor<R>
        >::type const
        operator | (quick_check_expr<L> const &l, phoenix::actor<R> const &r)
        {
            return proto::make_expr<proto::tag::bitwise_or, quick_check_domain>(l, r);
        }

        template<typename L, typename R>
        typename proto::result_of::make_expr<
            proto::tag::shift_right_assign
          , quick_check_domain
          , phoenix::actor<L>
          , quick_check_expr<R>
        >::type const
        operator >>= (phoenix::actor<L> const &l, quick_check_expr<R> const &r)
        {
            return proto::make_expr<proto::tag::shift_right_assign, quick_check_domain>(l, r);
        }
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
