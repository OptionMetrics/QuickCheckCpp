////////////////////////////////////////////////////////////////////////////////////////////////////
// \file property.cpp
// \brief A test of the property class
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;
namespace phx = boost::phoenix;

// Test for basic object semantics
void test_property_0()
{
    using namespace qchk;
    property<> prop0;
    property<> prop1 = prop0;
    prop1 = prop0;
}

void test_property_1()
{
    using namespace qchk;
    property<> prop2 = phx::val(true);
    auto config = make_config();
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
      , sout.str()
    );
}

void test_property_2()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");

    property<int> prop2 = _1 == 1866415935;
    auto config = make_config(_1 = di);
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "Falsifiable, after 2 tests:\n"
        "[-267902603]\n"
      , sout.str()
    );
}

void test_property_3()
{
    using namespace qchk;

    property<int, double> prop2 = _1 == _2;
    BOOST_CHECK(prop2(1, 1.0));

    fusion::vector2<int, double> args(42, 3.14);
    std::vector<std::string> classes = prop2.classifier()(args);
    BOOST_CHECK(classes.empty());
    BOOST_CHECK(prop2.condition()(args));
}

void test_property_4()
{
    using namespace qchk;

    property<int, double, short> prop2 = _1 == _2;
    BOOST_CHECK(prop2(1, 1.0, (short)0));
}

void test_property_5()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    property<int, double, grouped_by<int> > prop2 =
        _1 == _2 >>=
            group_by(_1 % 3)
              | classify(_1 < 0, "_1 negative")
              | classify(_2 < 0.0, "_2 negative")
              | (_1 == _2);
    BOOST_CHECK(prop2(-1, -1.0));
    BOOST_CHECK(!prop2(-1, 1.0));

    fusion::vector2<int, double> args0(1, 1.0);
    std::vector<std::string> classes = prop2.classifier()(args0);
    BOOST_CHECK(classes.empty());
    BOOST_CHECK(prop2.condition()(args0));
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the property class");

    test->add(BOOST_TEST_CASE(&test_property_0));
    test->add(BOOST_TEST_CASE(&test_property_1));
    test->add(BOOST_TEST_CASE(&test_property_2));
    test->add(BOOST_TEST_CASE(&test_property_3));
    test->add(BOOST_TEST_CASE(&test_property_4));
    test->add(BOOST_TEST_CASE(&test_property_5));

    return test;
}
