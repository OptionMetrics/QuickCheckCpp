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
#include <boost/preprocessor/enum_params_with_a_default.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>

#define QCHK_MAX_ARITY 10

#define QCHK_BOOST_NAMESPACE_BEGIN namespace boost {
#define QCHK_BOOST_NAMESPACE_END }
#define QCHK_BOOST_NAMESPACE boost

#define QCHK_RETURN(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace mpl = boost::mpl;
    namespace proto = boost::proto;
    namespace fusion = boost::fusion;
    namespace pheonix = boost::phoenix;

    using namespace phoenix::placeholders;

    template<typename Map, typename Rng>
    struct config;

    template<typename A, BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PP_DEC(QCHK_MAX_ARITY), typename B, void)>
    struct property;

}

QCHK_BOOST_NAMESPACE_END

#endif
