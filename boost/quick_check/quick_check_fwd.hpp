////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file quick_check_fwd.hpp
/// \brief Forward-declarations for quickCheckCpp
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QUICK_CHECK_HPP_FWD_INCLUDED
#define QCHK_QUICK_CHECK_HPP_FWD_INCLUDED

#include <boost/proto/proto.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/fusion/support/void.hpp>

/// \brief If the compiler doesn't support variadic templates, the
/// class templates defined in QuickCheckCpp support up to
/// \c QCHK_MAX_ARITY parameters.
///
/// The maximum number of template arguments that the class templates
/// \c quick_check::property<>, \c quick_check::qcheck_results<>, and
/// \c quick_check::qcheck_args<> accept. The default value is 10.
///
/// If the compiler supports variadic templates, this macro is ignored.
///
/// To increase the maximum template arity of QuickCheckCpp's class
/// templates, compile with \c QCHK_MAX_ARITY defined to something
/// greater than 10.
#ifndef QCHK_MAX_ARITY
#define QCHK_MAX_ARITY 10
#endif

#define QCHK_BOOST_NAMESPACE_BEGIN namespace boost {    ///< INTERNAL ONLY
#define QCHK_BOOST_NAMESPACE_END }                      ///< INTERNAL ONLY
#define QCHK_BOOST_NAMESPACE boost                      ///< INTERNAL ONLY

/// INTERNAL ONLY
#define QCHK_RETURN(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }
/// INTERNAL ONLY
#define QCHK_DOXY_DETAIL(...) __VA_ARGS__
/// INTERNAL ONLY
#define QCHK_DOXY_HIDDEN(...) __VA_ARGS__

namespace boost
{
    namespace random {}
}

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace mpl = boost::mpl;
    namespace proto = boost::proto;
    namespace fusion = boost::fusion;
    namespace pheonix = boost::phoenix;
    namespace random = boost::random;

    using namespace phoenix::placeholders;
    using phoenix::function;

    namespace detail
    {
        struct classify_;

        struct group_by_;

        template<typename T>
        struct array;

        struct qcheck_access;

        struct ungrouped_args;

        template<typename T>
        struct fusion_elem
          : mpl::if_<is_void<T>, fusion::void_, T>
        {};

        template<typename Property, typename Config>
        struct get_group_by_type;

        template<typename Args, typename GroupBy>
        struct make_qcheck_results_type;
    }

    template<typename Map, typename Rng>
    struct config;

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
    template<typename ...As>
    struct property;

    template<typename ...As>
    struct qcheck_results;

    template<typename ...As>
    struct qcheck_args;
#else
    template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
    struct property;

    template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
    struct qcheck_results;

    template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
    struct qcheck_args;
#endif

    template<typename>
    struct grouped_by;

    template<typename Property, typename Config>
    typename detail::make_qcheck_results_type<
        typename Config::result_type
      , typename detail::get_group_by_type<Property, Config>::type
    >::type
    qcheck(Property const &prop, Config &config);

    template<typename Property, typename Config>
    typename detail::make_qcheck_results_type<
        typename Config::result_type
      , typename detail::get_group_by_type<Property, Config>::type
    >::type
    qcheck(Property const &prop, Config &config, std::size_t sized);
}

QCHK_BOOST_NAMESPACE_END

#endif
