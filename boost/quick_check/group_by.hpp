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
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct group_by_
        {};

        struct ungrouped_args
        {
            template<typename Args>
            ungrouped_args operator()(Args &) const
            {
                return *this;
            }

            friend std::ostream &operator<<(std::ostream &sout, ungrouped_args)
            {
                return sout;
            }
        };

        template<typename Fun>
        struct group_args
        {
            group_args(Fun const &fun)
              : fun_(fun)
            {}

            template<typename Args>
            typename fusion::result_of::invoke_function_object<Fun, Args>::type
            operator()(Args &args) const
            {
                return fusion::invoke_function_object(this->fun_, args);
            }
        private:
            Fun fun_;
        };

        struct GetGrouperFromCombinators
          : proto::or_<
                proto::when<
                    Grouped
                  , group_args<boost::remove_reference<proto::_> >(proto::_)
                >
              , proto::when<
                    proto::bitwise_or<Grouped, Classify>
                  , group_args<boost::remove_reference<proto::_child(proto::_left)> >(proto::_child(proto::_left))
                >
              , proto::when<
                    proto::bitwise_or<GetGrouperFromCombinators, Classify>
                  , GetGrouperFromCombinators(proto::_left)
                >
            >
        {};

        struct GetGrouperFromCombinatorExpr
          : proto::or_<
                proto::when<
                    proto::bitwise_or<GetGrouperFromCombinators, phoenix::meta_grammar>
                  , GetGrouperFromCombinators(proto::_left)
                >
              , proto::when<
                    phoenix::meta_grammar
                  , proto::make<ungrouped_args>
                >
            >
        {};

        struct GetGrouperFromConditionalExpr
          : proto::when<
                proto::shift_right_assign<phoenix::meta_grammar, GetGrouperFromCombinatorExpr>
              , GetGrouperFromCombinatorExpr(proto::_right)
            >
        {};

        struct GetGrouper
          : proto::or_<
                GetGrouperFromConditionalExpr
              , GetGrouperFromCombinatorExpr
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<GetGrouper(Expr const &)>
        >::type
        get_grouper(Expr const & prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return GetGrouper()(prop);
        }

        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        typename property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::grouper_type const &
        get_grouper(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop.grouper();
        }
    }

    template<typename>
    struct grouped_by
    {};

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
