///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <boost/quick_check/quick_check.hpp>

namespace qchk = boost::quick_check;
using namespace qchk;

int main()
{
    // some generators
    uniform<int> die(1,6);
    normal<double> one(0.0, 1.0);

    // a QuickCheckCpp configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(_1 = die, _2 = one);

//[ClassifierExample
    // Here's a property to test:
    auto is_reflexive =
          classify(_2 > 0, "positive")
        | classify(_2 <= 0, "non-positive")
        | ((_1 + _2) == (_2 + _1)) ; // The property to test
//]

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary();
}
