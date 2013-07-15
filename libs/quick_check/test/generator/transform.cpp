////////////////////////////////////////////////////////////////////////////////////////////////////
// \file transform_gen.cpp
// \brief A test of the transform generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

static const std::size_t CLOOPS = 128;

namespace qchk = boost::quick_check;

void test_transform()
{
    file_dist<int> igen("uniform_int_distribution.txt");
    auto gen = qchk::transform(
        igen
      , qchk::detail::make_unary([](int i){return std::abs(i)%6 + 1;})
    );

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        int j = gen(gen);
        BOOST_CHECK_LE(j, 6);
        BOOST_CHECK_GE(j, 1);
    }
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the transform generator");

    test->add(BOOST_TEST_CASE(&test_transform));

    return test;
}
