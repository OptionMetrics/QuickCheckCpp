#include <cmath>
#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <boost/quick_check/quick_check.hpp>

namespace qchk = boost::quick_check;
using namespace qchk;

int main()
{
    // A source for random numbers
    boost::random::mt11213b rng;

    // some generators
    uniform<int> die(1,6);
    normal<double> one(0.0, 1.0);

    // a quickCheck configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(_1 = die,
                              _2 = one,
                              _rng = rng,
                              _test_count = 10000/*,
                              _max_test_count = 10000*/);

    // Here's a property to test:
    //property<int[3], double[3], grouped_by<int[3]> >
    auto
        is_reflexive = 
            (_2 > 0 ) >>=
                group_by(_1 % 3)
                   | classify(_1>3,"foo")
                   | classify(_1<=3,"bar")
                   | ((_1 + _2) == (_2 + _1)) ;

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    auto res = qcheck(is_reflexive, config);
    res.print_summary();
}
