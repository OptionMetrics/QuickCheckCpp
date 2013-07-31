////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck/test2.cpp
// \brief A test of the qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"
#include "../utility.hpp"

namespace qchk = boost::quick_check;
namespace proto = boost::proto;
namespace phx = boost::phoenix;

struct size_impl
{
    typedef std::size_t result_type;
    template<typename T>
    std::size_t operator()(T const &t) const
    {
        return t.size();
    }
};

phx::function<size_impl> size;

void test_qcheck_sized_auto_0()
{
    using namespace qchk;
    boost::random::mt11213b rng;

    auto ds = qchk::string();

    auto const prop2 = size(_1) < 100u;
    auto config = make_config(_1 = ds);
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
      , sout.str()
    );

    config.resized(200u);
    res = qcheck(prop2, config);
    BOOST_CHECK(!res.success());

    auto config2 = make_config(_1 = ds, _sized = 200u);
    auto res2 = qcheck(prop2, config2);
    BOOST_CHECK(!res2.success());
}

void test_qcheck_sized_prop_0()
{
    using namespace qchk;
    boost::random::mt11213b rng;

    auto ds = qchk::string();

    property<std::string> const prop2 = size(_1) < 100u;
    auto config = make_config(_1 = ds);
    auto res = qcheck(prop2, config);
    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
      , sout.str()
    );

    config.resized(200u);
    res = qcheck(prop2, config);
    BOOST_CHECK(!res.success());

    auto config2 = make_config(_1 = ds, _sized = 200u);
    auto res2 = qcheck(prop2, config2);
    BOOST_CHECK(!res2.success());
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the qcheck algorithm");

    test->add(BOOST_TEST_CASE(&test_qcheck_sized_auto_0));
    test->add(BOOST_TEST_CASE(&test_qcheck_sized_prop_0));

    return test;
}
