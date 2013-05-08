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
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/phoenix/core/domain.hpp>
#include <boost/phoenix/core/detail/function_eval.hpp>
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
            typedef std::string result_type;

            template<typename Args>
            std::string operator()(Args &) const
            {
                return std::string();
            }
        };

        template<typename Cond, typename Rest = unclassified_args>
        struct classify_args
        {
            typedef std::string result_type;

            classify_args(Cond const &cond, std::string const &name, Rest const &rest = Rest())
              : cond_(cond)
              , name_(name)
              , rest_(rest)
            {}

            template<typename Args>
            std::string operator()(Args &args) const
            {
                if(static_cast<bool>(fusion::invoke_function_object(this->cond_, args)))
                    return this->name_;
                return this->rest_(args);
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

        struct FoldClassifiers
          : proto::or_<
                proto::when<
                    proto::bitwise_or<FoldClassifiers, FoldClassifiers>
                  , FoldClassifiers(proto::_left, FoldClassifiers(proto::_right))
                >
              , proto::when<
                    Classify
                  , make_classify_args(
                        proto::_child1
                      , proto::_value(proto::_child2)
                      , proto::_state
                    )
                >
            >
        {};

        struct GetClassifier
          : proto::or_<
                proto::when<
                    proto::bitwise_or<FoldClassifiers, phoenix::meta_grammar>
                  , FoldClassifiers(proto::_left, unclassified_args())
                >
              , proto::when<
                    phoenix::meta_grammar
                  , unclassified_args()
                >
            >
        {};

        template<typename Expr>
        typename boost::result_of<GetClassifier(phoenix::actor<Expr> const &)>::type
        get_classifier(phoenix::actor<Expr> const & prop)
        {
            return GetClassifier()(prop);
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
        phoenix::detail::tag::function_eval
      , phoenix::phoenix_domain
      , detail::classify_
      , Expr
      , std::string
    >::type const
    classify(Expr const &e, std::string const& name)
    {
        return proto::make_expr<phoenix::detail::tag::function_eval, phoenix::phoenix_domain>(
            detail::classify_()
          , e
          , name
        );
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
