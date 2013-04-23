////////////////////////////////////////////////////////////////////////////////////////////////////
// \file quick_check_fwd.hpp
// \brief Forward-declarations for quickCheckCPP
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_QUICK_CHECK_QUICK_CHECK_HPP_FWD_INCLUDED
#define BOOST_QUICK_CHECK_QUICK_CHECK_HPP_FWD_INCLUDED

#include <boost/proto/proto.hpp>
#include <boost/phoenix/core.hpp>

#define QKCK_BOOST_NAMESPACE_BEGIN namespace boost {
#define QKCK_BOOST_NAMESPACE_END }
#define QKCK_BOOST_NAMESPACE boost

QKCK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace mpl = boost::mpl;
    namespace proto = boost::proto;
    namespace fusion = boost::fusion;
    namespace pheonix = boost::phoenix;

    using namespace phoenix::placeholders;
}

QKCK_BOOST_NAMESPACE_END

#endif
