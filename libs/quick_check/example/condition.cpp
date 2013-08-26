///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////////////////////////

//[ConditionExample
#include <cmath>
#include <cassert>
#include <iostream>
#include <boost/quick_check/quick_check.hpp>

namespace qchk = boost::quick_check;
using namespace qchk;

namespace lazy
{
    struct sqrt_impl
    {
        typedef double result_type;
        double operator()(double d) const
        {
            assert(d > 0);
            return std::sqrt(d);
        }
    };

    struct abs_impl
    {
        typedef double result_type;
        double operator()(double d) const
        {
            return std::abs(d);
        }
    };

    //<-
    #if defined(BOOST_NO_CXX11_CONSTEXPR) || defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
    const function<sqrt_impl> sqrt((sqrt_impl())) ;
    const function<abs_impl> abs((abs_impl())) ;
    #else
    constexpr function<sqrt_impl> sqrt {};
    constexpr function<abs_impl> abs {};
    #endif
    //->
    //=constexpr function<sqrt_impl> sqrt {};
    //=constexpr function<abs_impl> abs {};
}

int main()
{
    normal<double> one(0.0, 1.0);
    auto config = make_config(_1 = one, _test_count = 10000);

    // Test that squaring the result of sqrt gets us approximately back to
    // where we started.
    auto sqrt_prop =
        (_1 > 0) >>= // Skip all non-positive numbers.
            (lazy::abs(_1 - lazy::sqrt(_1) * lazy::sqrt(_1)) <= DBL_EPSILON) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(sqrt_prop, config);
    res.print_summary();
}
//]
