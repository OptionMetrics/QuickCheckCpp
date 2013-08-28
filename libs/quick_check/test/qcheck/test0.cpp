////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file qcheck/test0.cpp
/// \brief A test of the qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

namespace qchk = boost::quick_check;
namespace proto = boost::proto;
namespace phx = boost::phoenix;

void test_qcheck_true_auto_0()
{
    using namespace qchk;
    auto prop2 = phx::val(true);
    auto config = make_config();
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
      , sout.str()
    );
}

void test_qcheck_false_auto_0()
{
    using namespace qchk;
    auto prop2 = phx::val(false);
    auto config = make_config();
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "Falsifiable, after 1 tests:\n"
        "[]\n"
      , sout.str()
    );
}

void test_qcheck_true_prop_0()
{
    using namespace qchk;
    property<> prop2 = phx::val(true);
    auto config = make_config();
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
      , sout.str()
    );
}

void test_qcheck_false_prop_0()
{
    using namespace qchk;
    property<> prop2 = phx::val(false);
    auto config = make_config();
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "Falsifiable, after 1 tests:\n"
        "[]\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the qcheck algorithm");

    test->add(BOOST_TEST_CASE(&test_qcheck_true_auto_0));
    test->add(BOOST_TEST_CASE(&test_qcheck_false_auto_0));
    test->add(BOOST_TEST_CASE(&test_qcheck_true_prop_0));
    test->add(BOOST_TEST_CASE(&test_qcheck_false_prop_0));

    return test;
}
