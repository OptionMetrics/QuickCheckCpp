///////////////////////////////////////////////////////////////////////////////////////////////////
// \file grammar.hpp
// \brief Definition of the quick_check EDSL's grammar
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_GRAMMAR_HPP_INCLUDED
#define QCHK_DETAIL_GRAMMAR_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/proto/proto.hpp>
#include <boost/phoenix/core/meta_grammar.hpp>
#include <boost/phoenix/core/arity.hpp>
#include <boost/phoenix/core/detail/function_eval.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        // group_by(_1 % 5)
        struct GroupBy
          : proto::nary_expr<
                phoenix::detail::tag::function_eval
              , proto::terminal<detail::group_by_>
              , phoenix::meta_grammar
            >
        {};

        // classify(_1>_2, "greater")
        struct Classify
          : proto::nary_expr<
                phoenix::detail::tag::function_eval
              , proto::terminal<detail::classify_>
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
                proto::bitwise_or<GroupBy, Classify>
              , proto::bitwise_or<GroupedClassifiers, Classify>
            >
        {};

        // All valid combinations of combinators
        struct Combinators
          : proto::or_<
                GroupBy
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
                proto::shift_right_assign<phoenix::meta_grammar, CombinatorExpr>
              , CombinatorExpr
            >
        {};

    }
}

QCHK_BOOST_NAMESPACE_END

#endif
