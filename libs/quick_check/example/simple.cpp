//[SimpleExample
#include <iostream>
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

    // a QuickCheckCpp configuration. Placeholders _1
    // and _2 receive values generated from die and one:
    auto config = make_config(_1 = die,
                              _2 = one,
                              _rng = rng,               // Specify a custom random number generator
                              _test_count = 10000,      // The maximum number of tests to execute
                              _max_test_count = 15000); // The maximum number of inputs to generate

    // Here's a property to test:
    /*<< Rather than `auto`, you could use `property<int, double, grouped_by<int> >` here. >>*/
    auto
        is_reflexive =
            (_2 > 0) >>=                        // A condition
                group_by(_1 % 3)                // Input grouping
                   | classify(_1>3,"foo")       // Input classification
                   | classify(_1<=3,"bar")      //
                   | ((_1 + _2) == (_2 + _1)) ; // The property to test

    // Test the property. Res is a results object that
    // holds the values that caused the test to fail
    /*<< Rather than `auto`, you could use `qcheck_results<int, double, grouped_by<int> >` here. >>*/
    auto res = qcheck(is_reflexive, config);
    res.print_summary();
}
//]
