////////////////////////////////////////////////////////////////////////////////////////////////////
// \file group_by.cpp
// \brief A test of the group_by modifier
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

static const std::size_t CLOOPS = 128;

namespace qchk = boost::quick_check;

void test_group_by_auto_1()
{
    using namespace qchk;
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
    auto
        is_reflexive =
            group_by(_1 % 3)
          | ((_1 + _2) == (_2 + _1)) ;

    proto::assert_matches<qchk::detail::QuickCheckExpr>(is_reflexive);

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    std::stringstream sout;
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "16% -2.\n"
        "19% -1.\n"
        "35% 0.\n"
        "17% 1.\n"
        "13% 2.\n"
      , sout.str()
    );
}

void test_group_by_prop_1()
{
    using namespace qchk;
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
    property<int, double, grouped_by<int> >
        is_reflexive =
            group_by(_1 % 3)
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    std::stringstream sout;
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "16% -2.\n"
        "19% -1.\n"
        "35% 0.\n"
        "17% 1.\n"
        "13% 2.\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the group_by modifier");

    test->add(BOOST_TEST_CASE(&test_group_by_auto_1));
    test->add(BOOST_TEST_CASE(&test_group_by_prop_1));

    return test;
}
