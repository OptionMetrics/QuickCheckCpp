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
        struct Classify
          : proto::nary_expr<
                phoenix::detail::tag::function_eval
              , proto::terminal<detail::classify_>
              , phoenix::meta_grammar
              , proto::terminal<std::string>
            >
        {};

        struct Classifiers
          : proto::or_<
                proto::bitwise_or<Classifiers, Classifiers>
              , Classify
            >
        {};

        struct ClassifiedExpr
          : proto::bitwise_or<Classifiers, phoenix::meta_grammar>
        {};

    }
}

QCHK_BOOST_NAMESPACE_END

#endif
