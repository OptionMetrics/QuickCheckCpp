///////////////////////////////////////////////////////////////////////////////////////////////////
// \file condition.hpp
// \brief Definition of the condition grammar element
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_CONDITION_HPP_INCLUDED
#define QCHK_CONDITION_HPP_INCLUDED

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
        struct unconditional
        {
            template<typename Args>
            bool operator()(Args &) const
            {
                return true;
            }
        };

        template<typename Fun>
        struct condition_args
        {
            condition_args(Fun const &fun)
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

        struct GetCondition
          : proto::or_<
                proto::when<
                    proto::shift_right_assign<phoenix::meta_grammar, proto::_>
                  , condition_args<boost::remove_reference<proto::_left> >(proto::_left)
                >
              , proto::otherwise<
                    proto::make<unconditional>
                >
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<GetCondition(Expr const &)>
        >::type
        get_condition(Expr const & prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return GetCondition()(prop);
        }

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
        template<typename...As>
        typename property<As...>::condition_type const &
        get_condition(property<As...> const &prop)
        {
            return prop.condition();
        }
#else
        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        typename property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::condition_type const &
        get_condition(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop.condition();
        }
#endif
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
