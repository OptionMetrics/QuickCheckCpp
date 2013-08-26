///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////////////////////////

//[BindExample
#include <iostream>
#include <boost/quick_check/quick_check.hpp>

namespace qchk = boost::quick_check;
using namespace qchk;

namespace my_library
{
    // An ordinary C++ function, which can be arbitrarily complicated.
    // It need not be a template.
    template<typename A, typename B>
    bool is_reflexive(A a, B b)
    {
        return (a + b) == (b + a);
    }
}

namespace lazy
{
    // Define a function object wrapper
    struct is_reflexive_impl
    {
        typedef bool result_type;

        template<typename A, typename B>
        bool operator()(A a, B b) const
        {
            return my_library::is_reflexive(a, b);
        }
    };

    // Define the lazy Phoenix is_reflexive function object, using qchk::function,
    // which is an alias for boost::phoenix::function.
    //<-
#if defined(BOOST_NO_CXX11_CONSTEXPR) || defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
    const function<is_reflexive_impl> is_reflexive((is_reflexive_impl())) ;
#else
    constexpr function<is_reflexive_impl> is_reflexive {};
#endif
    //->
    //=constexpr function<is_reflexive_impl> is_reflexive {};
}

int main()
{
    // some generators
    uniform<int> die(1,6);
    normal<double> one(0.0, 1.0);

    // a QuickCheckCpp configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(_1 = die, _2 = one);

    // Here's a property to test:
    auto is_reflexive_prop = lazy::is_reflexive(_1, _2); // The property to test

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive_prop, config);
    res.print_summary();
}
//]
