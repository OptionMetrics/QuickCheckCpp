////////////////////////////////////////////////////////////////////////////////////////////////////
// \file condition.cpp
// \brief A test of qcheck functionality with conditions
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;
namespace phx = boost::phoenix;

bool throw_on_non_positive(int i, double d)
{
    if(i <= 0)
        throw std::runtime_error("non-positive");
    return true;
}

void test_condition_throw_auto()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = di, _2 = dd);

    // Here's a property to test:
    auto
        throw_if =
            phx::bind(&throw_on_non_positive, _1, _2) ;

    BOOST_CHECK_THROW(qcheck(throw_if, config), std::runtime_error);
}

void test_condition_safe_auto()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = di, _2 = dd);

    // Here's a property to test:
    auto
        throw_if =
            (_1 > 0) >>=
                phx::bind(&throw_on_non_positive, _1, _2) ;

    BOOST_CHECK_NO_THROW(qcheck(throw_if, config));
}

void test_condition_throw_prop()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = di, _2 = dd);

    // Here's a property to test:
    property<int, double>
        throw_if =
            phx::bind(&throw_on_non_positive, _1, _2) ;

    BOOST_CHECK_THROW(qcheck(throw_if, config), std::runtime_error);
}

void test_condition_safe_prop()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = di, _2 = dd);

    // Here's a property to test:
    property<int, double>
        throw_if =
            (_1 > 0) >>=
                phx::bind(&throw_on_non_positive, _1, _2) ;

    BOOST_CHECK_NO_THROW(qcheck(throw_if, config));
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("test for input conditions");

    test->add(BOOST_TEST_CASE(&test_condition_throw_auto));
    test->add(BOOST_TEST_CASE(&test_condition_safe_auto));
    test->add(BOOST_TEST_CASE(&test_condition_throw_prop));
    test->add(BOOST_TEST_CASE(&test_condition_safe_prop));

    return test;
}
