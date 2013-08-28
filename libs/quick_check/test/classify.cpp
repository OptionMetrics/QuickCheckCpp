////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file classify.cpp
/// \brief A test of the classify modifier
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
#include "./file_dist.hpp"

static const std::size_t CLOOPS = 128;

namespace qchk = boost::quick_check;

void test_classify_auto_1()
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

    std::stringstream sout;
    // Here's a property to test:
    auto
        is_reflexive =
            classify(_2<0.,"bar")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "51% bar.\n"
      , sout.str()
    );
}

void test_classify_auto_2()
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

    std::stringstream sout;
    // Here's a property to test:
    auto
        is_reflexive =
            classify(_2>0.,"foo")
          | classify(_2<=0.,"bar")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "51% bar.\n"
        "49% foo.\n"
      , sout.str()
    );
}

void test_classify_auto_3()
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

    std::stringstream sout;
    // Here's a property to test:
    auto
        is_reflexive =
            classify(_2>0.,"foo")
          | classify(_2<=0.,"bar")
          | classify(_1>0,"baz")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "28% bar.\n"
        "23% bar, baz.\n"
        "26% baz, foo.\n"
        "23% foo.\n"
      , sout.str()
    );
}

void test_classify_prop_1()
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

    std::stringstream sout;
    // Here's a property to test:
    property<int, double>
        is_reflexive =
            classify(_2<0.,"bar")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "51% bar.\n"
      , sout.str()
    );
}

void test_classify_prop_2()
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

    std::stringstream sout;
    // Here's a property to test:
    property<int, double>
        is_reflexive =
            classify(_2>0.,"foo")
          | classify(_2<=0.,"bar")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "51% bar.\n"
        "49% foo.\n"
      , sout.str()
    );
}

void test_classify_prop_3()
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

    std::stringstream sout;
    // Here's a property to test:
    property<int, double>
        is_reflexive =
            classify(_2>0.,"foo")
          | classify(_2<=0.,"bar")
          | classify(_1>0,"baz")
          | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary(sout);

    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "28% bar.\n"
        "23% bar, baz.\n"
        "26% baz, foo.\n"
        "23% foo.\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the classify modifier");

    test->add(BOOST_TEST_CASE(&test_classify_auto_1));
    test->add(BOOST_TEST_CASE(&test_classify_auto_2));
    test->add(BOOST_TEST_CASE(&test_classify_auto_3));
    test->add(BOOST_TEST_CASE(&test_classify_prop_1));
    test->add(BOOST_TEST_CASE(&test_classify_prop_2));
    test->add(BOOST_TEST_CASE(&test_classify_prop_3));

    return test;
}
