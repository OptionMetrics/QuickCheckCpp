///////////////////////////////////////////////////////////////////////////////////////////////////
// \file classify.hpp
// \brief Definition of the classify grammar element
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_CLASSIFY_HPP_INCLUDED
#define QCHK_CLASSIFY_HPP_INCLUDED

#include <string>
#include <vector>
#include <algorithm>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/quick_check/detail/grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct classify_
        {};

        struct unclassified_args
        {
            typedef std::vector<std::string> result_type;

            template<typename Args>
            result_type operator()(Args &) const
            {
                return result_type();
            }
        };

        template<typename Cond, typename Rest = unclassified_args>
        struct classify_args
        {
            typedef std::vector<std::string> result_type;

            classify_args(Cond const &cond, std::string const &name, Rest const &rest = Rest())
              : cond_(cond)
              , name_(name)
              , rest_(rest)
            {}

            template<typename Args>
            result_type operator()(Args &args) const
            {
                result_type res = this->rest_(args);
                if(static_cast<bool>(fusion::invoke_function_object(this->cond_, args)))
                {
                    // Keep the vector sorted as we build it.
                    auto it = std::lower_bound(res.begin(), res.end(), this->name_);
                    res.insert(it, this->name_);
                }
                return res;
            }

        private:
            Cond cond_;
            std::string name_;
            Rest rest_;
        };

        struct make_classify_args : proto::callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Cond, typename Name, typename Rest>
            struct result<This(Cond, Name, Rest)>
            {
                typedef
                    classify_args<
                        typename boost::remove_const<typename remove_reference<Cond>::type>::type
                      , typename boost::remove_const<typename remove_reference<Rest>::type>::type
                    >
                type;
            };

            template<typename Cond, typename Rest>
            classify_args<Cond, Rest>
            operator()(Cond const &cond, std::string const &name, Rest const &rest) const
            {
                return classify_args<Cond, Rest>(cond, name, rest);
            }
        };

        struct GetClassifiers
          : proto::or_<
                proto::when<
                    Classify
                  , make_classify_args(
                        proto::_child0
                      , proto::_value(proto::_child1)
                      , proto::_state
                    )
                >
              , proto::when<
                    proto::terminal<proto::_>
                  , proto::_state
                >
              , proto::otherwise<
                    proto::fold<proto::_, proto::_state, GetClassifiers>
                >
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<GetClassifiers(Expr const &, unclassified_args const &)>
        >::type
        get_classifier(Expr const & prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return GetClassifiers()(prop, unclassified_args());
        }

        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        typename property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::classifier_type const &
        get_classifier(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop.classifier();
        }
    }

    template<typename Expr>
    typename proto::result_of::make_expr<
        detail::classify_
      , detail::quick_check_domain
      , phoenix::actor<Expr>
      , std::string
    >::type const
    classify(phoenix::actor<Expr> const &e, std::string const& name)
    {
        return proto::make_expr<detail::classify_, detail::quick_check_domain>(e, name);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
