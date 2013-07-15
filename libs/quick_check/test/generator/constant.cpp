////////////////////////////////////////////////////////////////////////////////////////////////////
// \file constant_gen.cpp
// \brief A test for the constant generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>

namespace qchk = boost::quick_check;

void test_constant()
{
    const int value = 42;
    boost::random::mt11213b rng;
    auto gen = qchk::constant(value);
    for (int i = 0; i < 500; ++i)
        BOOST_CHECK_EQUAL(gen(rng), value);
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("basic tests for constant generator");

    test->add(BOOST_TEST_CASE(&test_constant));

    return test;
}
