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
#include <boost/quick_check/classify.hpp>
#include <boost/quick_check/group_by.hpp>
#include <boost/quick_check/condition.hpp>

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
                  , GetPropertyFromCombinatorExpr(proto::_right)
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
            template<typename QchkResults, typename Args, typename Group>
            static void add_failure(
                QchkResults &results
              , Args &args
              , std::vector<std::string> const &classes
              , Group const &group
            )
            {
                results.add_failure(args, classes, group);
            }

            template<typename QchkResults, typename Group>
            static void add_success(
                QchkResults &results
              , std::vector<std::string> const &classes
              , Group const &group
            )
            {
                results.add_success(classes, group);
            }

            template<typename QchkResults>
            static void exhausted(QchkResults &results)
            {
                results.exhausted();
            }
        };

        template<typename Property, typename Config>
        struct get_group_by_type
        {
            typedef
                decltype(
                    detail::get_grouper(
                        boost::declval<Property const &>()
                    )(boost::declval<Config &>().gen())
                )
            type;
        };
    }

    template<typename Property, typename Config>
    typename detail::make_qcheck_results_type<
        typename Config::args_type
      , typename detail::get_group_by_type<Property, Config>::type
    >::type
    qcheck(Property const &prop_, Config &config)
    {
        typedef
            typename detail::make_qcheck_results_type<
                typename Config::args_type
              , typename detail::get_group_by_type<Property, Config>::type
            >::type
        results_type;

        results_type results;

        auto const &prop = detail::get_property(prop_);
        auto const &classify = detail::get_classifier(prop_);
        auto const &groupby = detail::get_grouper(prop_);
        auto const &condition = detail::get_condition(prop_);

        std::size_t n = 0, total = 0;
        for(; n < config.test_count() && total < config.max_test_count(); ++total)
        {
            auto args = config.gen();

            // Skip this if it is an invalid set of arguments
            if(!static_cast<bool>(condition(args)))
                continue;

            ++n; // ok, we've got a valid set of arguments
            auto classes = classify(args);
            auto group = groupby(args);

            // The static_cast here is mostly historical, but left here for
            // the sake of strictness.
            if(!static_cast<bool>(fusion::invoke_function_object(prop, args)))
            {
                detail::qcheck_access::add_failure(
                    results
                  , fusion::as_vector(fusion::transform(args, detail::unpack_array()))
                  , std::move(classes)
                  , std::move(group)
                );
            }
            else
            {
                detail::qcheck_access::add_success(
                    results
                  , std::move(classes)
                  , std::move(group)
                );
            }
        }

        // Record whether we had to bail early.
        if(n != config.test_count())
            detail::qcheck_access::exhausted(results);

        return results;
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
