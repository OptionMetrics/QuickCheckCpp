////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QCHECK_HPP_INCLUDED
#define QCHK_QCHECK_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/quick_check/classify.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/quick_check/detail/array.hpp>
#include <boost/quick_check/detail/grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct GetPropertyFromCombinatorExpr
          : proto::or_<
                proto::when<
                    proto::bitwise_or<Combinators, phoenix::meta_grammar>
                  , proto::_right
                >
              , proto::otherwise<
                    proto::_
                >
            >
        {};

        struct GetProperty
          : proto::or_<
                proto::when<
                    ConditionalExpr
                  , CombinatorExpr(proto::_right)
                >
              , proto::otherwise<
                    GetPropertyFromCombinatorExpr
                >
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<detail::GetProperty(Expr const &)>
        >::type
        get_property(Expr const &prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return detail::GetProperty()(prop);
        }

        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &
        get_property(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop;
        }

        struct qcheck_access
        {
            template<typename QchkResults, typename Args>
            static void add_failure(QchkResults &results, Args &args, std::string const &classname)
            {
                results.failures_.push_back(typename QchkResults::args_type(args, classname));
            }
        };
    }

    template<typename Property, typename Config>
    typename detail::make_qcheck_results_type<typename Config::args_type>::type
    qcheck(Property const &prop_, Config &config)
    {
        typedef typename
            detail::make_qcheck_results_type<typename Config::args_type>::type
        results_type;

        results_type results;

        auto const &prop = detail::get_property(prop_);
        auto const &classify = detail::get_classifier(prop_);
        auto const &groupby = detail::get_grouper(prop_);

        for(std::size_t n = 0; n < config.test_count(); ++n)
        {
            auto args = config.gen();

            // The static_cast here is so that operator! doesn't get invoked
            // for detail::array, which returns a new array that is the logical
            // negation of each element.
            if(!static_cast<bool>(fusion::invoke_function_object(prop, args)))
            {
                std::string classname = classify(args);
                auto group = groupby(args);
                detail::qcheck_access::add_failure(
                    results
                  , fusion::as_vector(fusion::transform(args, detail::unpack_array()))
                  , classname
                );
            }
        }

        return results;
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
