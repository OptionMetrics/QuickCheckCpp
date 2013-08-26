////////////////////////////////////////////////////////////////////////////////////////////////////
// \file zip_gen.cpp
// \brief A test of the zip generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

static const std::size_t CLOOPS = 128;

namespace qchk = boost::quick_check;

void test_zip()
{
    boost::random::mt11213b rng;

    file_dist<int> igen("uniform_int_distribution.txt");
    file_dist<double> dgen("normal_double_distribution.txt");
    auto dpos = qchk::transform(
        dgen
      , qchk::detail::make_unary([](double d){return std::abs(d);})
    );
    auto ipos = qchk::transform(
        igen
      , qchk::detail::make_unary([](int i){return std::abs(i);})
    );
    auto zip = qchk::zip(ipos, dpos);
    set_size_adl(zip, 12); // no-op, make sure it compiles

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::pair<int, double> p = zip(rng);
        BOOST_CHECK_GE(p.first, 0);
        BOOST_CHECK_GE(p.second, 0.0);
    }
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the zip generator");

    test->add(BOOST_TEST_CASE(&test_zip));

    return test;
}
