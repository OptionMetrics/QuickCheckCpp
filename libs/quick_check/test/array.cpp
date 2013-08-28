////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file array.cpp
/// \brief A basic tests with arrays
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;

void test_qchk_auto()
{
    using namespace qchk;
    std::stringstream sout;
    file_dist<int[3]> di("uniform_int_distribution.txt");
    file_dist<double[3]> dd("normal_double_distribution.txt");

    // a quickCheck configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(
        _1 = di,
        _2 = dd,
        _max_test_count = 10000
    );

    int izero[3] = {0,0,0};
    double dzero[3] = {0.,0.,0.};

    // Here's a property to test:
    auto
        is_reflexive =
            (_1 > izero) >>=
                group_by(_1[0] % 3)
                   | classify(_2>dzero,"foo")
                   | classify(_2<dzero,"bar")
                   | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "19% 0, bar.\n"
        "17% 0, foo.\n"
        "16% 1, bar.\n"
        "16% 1, foo.\n"
        "19% 2, bar.\n"
        "13% 2, foo.\n"
      , sout.str()
    );
}

void test_qchk_prop()
{
    using namespace qchk;
    std::stringstream sout;
    file_dist<int[3]> di("uniform_int_distribution.txt");
    file_dist<double[3]> dd("normal_double_distribution.txt");

    // a quickCheck configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(
        _1 = di,
        _2 = dd,
        _max_test_count = 10000
    );

    int izero[3] = {0,0,0};
    double dzero[3] = {0.,0.,0.};

    // Here's a property to test:
    property<int[3], double[3], grouped_by<int> >
        is_reflexive =
            (_1 > izero) >>=
                group_by(_1[0] % 3)
                   | classify(_2>dzero,"foo")
                   | classify(_2<dzero,"bar")
                   | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "19% 0, bar.\n"
        "17% 0, foo.\n"
        "16% 1, bar.\n"
        "16% 1, foo.\n"
        "19% 2, bar.\n"
        "13% 2, foo.\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("basic tests for qcheck with arrays");

    test->add(BOOST_TEST_CASE(&test_qchk_auto));
    test->add(BOOST_TEST_CASE(&test_qchk_prop));

    return test;
}
