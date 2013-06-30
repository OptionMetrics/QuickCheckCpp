////////////////////////////////////////////////////////////////////////////////////////////////////
// \file basic.cpp
// \brief A truly basic test of qcheck functionality
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <type_traits>
#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;

void test_qchk()
{
    using namespace qchk;
    std::stringstream sout;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    // a quickCheck configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(
        _1 = di,
        _2 = dd,
        _max_test_count = 10000
    );

    // Here's a property to test:
    //property<int[3], double[3], grouped_by<int[3]> >
    auto
        is_reflexive = 
            (_1 > 0) >>=
                group_by(_1 % 3)
                   | classify(_2>0,"foo")
                   | classify(_2<0,"bar")
                   | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "19% 0, bar.\n"
        "17% 0, foo.\n"
        "17% 1, bar.\n"
        "16% 1, foo.\n"
        "16% 2, bar.\n"
        "15% 2, foo.\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("basic tests for qcheck");

    test->add(BOOST_TEST_CASE(&test_qchk));

    return test;
}
