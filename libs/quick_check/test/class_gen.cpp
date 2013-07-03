////////////////////////////////////////////////////////////////////////////////////////////////////
// \file class_gen.cpp
// \brief A test of the class generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;

int ctor1 = 0;
int ctor2 = 0;

struct S
{
    S(int, double)
    {
        ++ctor1;
    }

    S(int)
    {
        ++ctor2;
    }
};

void test_class()
{
    static int const cloops = 500;
    // A source for random numbers
    boost::random::mt11213b rng;
    qchk::uniform<int> i(1,6);
    qchk::normal<double> d(0.,1.);

    auto genS0 = qchk::class_<S>(i, d);

    for(int i = 0; i < 500; ++i)
    {
        S s = genS0(rng);
        (void)s;
    }

    BOOST_CHECK_EQUAL(ctor1, cloops);
    ctor1 = 0;

    auto genS1 = qchk::class_<S>( qchk::ctor(i, d) %50
                                | qchk::ctor(i)    %50
                                );

    for(int i = 0; i < 500; ++i)
    {
        S s = genS1(rng);
        (void)s;
    }

    BOOST_CHECK_EQUAL(ctor1 + ctor2, cloops);
    BOOST_CHECK_NE(ctor1, 0);
    BOOST_CHECK_NE(ctor2, 0);
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the class generator");

    test->add(BOOST_TEST_CASE(&test_class));

    return test;
}
