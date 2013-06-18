////////////////////////////////////////////////////////////////////////////////////////////////////
// \file quick_check_fwd.hpp
// \brief Forward-declarations for quickCheckCPP
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QUICK_CHECK_HPP_FWD_INCLUDED
#define QCHK_QUICK_CHECK_HPP_FWD_INCLUDED

#include <boost/proto/proto.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/fusion/support/void.hpp>

#define QCHK_MAX_ARITY 10

#define QCHK_BOOST_NAMESPACE_BEGIN namespace boost {
#define QCHK_BOOST_NAMESPACE_END }
#define QCHK_BOOST_NAMESPACE boost

#define QCHK_RETURN(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

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
    }

    template<typename Map, typename Rng>
    struct config;

    template<typename A BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY), typename B, = void BOOST_PP_INTERCEPT)>
    struct property;

    template<typename A BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY), typename B, = void BOOST_PP_INTERCEPT)>
    struct qcheck_results;

    template<typename A BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY), typename B, = void BOOST_PP_INTERCEPT)>
    struct qcheck_args;

    template<typename>
    struct grouped_by;
}

QCHK_BOOST_NAMESPACE_END

#endif
