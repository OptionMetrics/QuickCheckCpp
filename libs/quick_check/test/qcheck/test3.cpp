////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck/test3.cpp
// \brief A test of the qcheck algorithm
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
#include "../utility.hpp"

namespace qchk = boost::quick_check;
namespace proto = boost::proto;
namespace phx = boost::phoenix;

void test_qcheck_test_count_auto_0()
{
    using namespace qchk;

    file_dist<int> igen(qcheck_test::abs_test_root("uniform_int_distribution.txt"));
    file_dist<double> dgen(qcheck_test::abs_test_root("normal_double_distribution.txt"));

    auto const prop2 = ((_1+_2) == (_2+_1));
    auto config = make_config(_1 = igen, _2 = dgen, _test_count = 1000u);
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 1000 tests.\n"
      , sout.str()
    );
}

void test_qcheck_test_count_prop_0()
{
    using namespace qchk;

    file_dist<int> igen(qcheck_test::abs_test_root("uniform_int_distribution.txt"));
    file_dist<double> dgen(qcheck_test::abs_test_root("normal_double_distribution.txt"));

    property<int, double> const prop2 = ((_1+_2) == (_2+_1));
    auto config = make_config(_1 = igen, _2 = dgen, _test_count = 1000u);
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 1000 tests.\n"
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

    test->add(BOOST_TEST_CASE(&test_qcheck_test_count_auto_0));
    test->add(BOOST_TEST_CASE(&test_qcheck_test_count_prop_0));

    return test;
}
