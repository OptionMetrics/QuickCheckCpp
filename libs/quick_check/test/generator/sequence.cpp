////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file sequence.cpp
/// \brief A test of the sequence generator
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
#include <list>
#include <vector>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

static const std::size_t CLOOPS = 128;

namespace qchk = boost::quick_check;

void test_sequence()
{
    boost::random::mt11213b rng;

    file_dist<int> igen("uniform_int_distribution.txt");
    auto die = qchk::transform(
        igen
      , qchk::detail::make_unary([](int i){return std::abs(i)%6 + 1;})
    );
    auto rgdie = qchk::sequence<std::list<int> >(die);
    set_size_adl(rgdie, 12);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::list<int> v = rgdie(rng);
        BOOST_CHECK_LT(v.size(), 12u);
        for(int j : v)
        {
            BOOST_CHECK_LE(j, 6);
            BOOST_CHECK_GE(j, 1);
        }
    }
}

void test_ordered_sequence()
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
    auto rgzip =
        qchk::ordered_sequence<std::vector<std::pair<int, double> > >(qchk::zip(ipos, dpos));
    set_size_adl(rgzip, 12);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::vector<std::pair<int, double> > v = rgzip(rng);
        BOOST_CHECK_LT(v.size(), 12u);
        for(auto j : v)
        {
            BOOST_CHECK_GE(j.first, 0);
            BOOST_CHECK_GE(j.second, 0.0);
        }
        BOOST_CHECK(std::is_sorted(v.begin(), v.end()));
    }
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the sequence generator");

    test->add(BOOST_TEST_CASE(&test_sequence));
    test->add(BOOST_TEST_CASE(&test_ordered_sequence));

    return test;
}
