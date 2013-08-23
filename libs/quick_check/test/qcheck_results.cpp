////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck_results.cpp
// \brief A test of the qcheck_results struct
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/config.hpp>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "./file_dist.hpp"
#include "./utility.hpp"

namespace qchk = boost::quick_check;
namespace proto = boost::proto;
namespace phx = boost::phoenix;

#if !defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS) && \
    !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#include <type_traits>
static_assert(
    std::has_move_constructor<qchk::qcheck_results<int, double, qchk::grouped_by<int> > >::value,
    "should have move constructor");
#endif

template<typename T>
void foo(T const &, T const &) {}

void test_qcheck_results_0()
{
    using namespace qchk;

    file_dist<int> igen(qcheck_test::abs_test_root("uniform_int_distribution.txt"));
    file_dist<double> dgen(qcheck_test::abs_test_root("normal_double_distribution.txt"));

    // default construction
    qcheck_results<int, double, grouped_by<int> > qr0;
    BOOST_CHECK(qr0);
    BOOST_CHECK(qr0.success());
    BOOST_CHECK(!qr0.exhausted());
    BOOST_CHECK(qr0.failures().empty());

    // copy construction
    qcheck_results<int, double, grouped_by<int> > qr1 = qr0;
    BOOST_CHECK(qr1);
    BOOST_CHECK(qr1.success());
    BOOST_CHECK(!qr1.exhausted());
    BOOST_CHECK(qr1.failures().empty());

    // copy assignment
    qr1 = qr0;
    BOOST_CHECK(qr1);
    BOOST_CHECK(qr1.success());
    BOOST_CHECK(!qr1.exhausted());
    BOOST_CHECK(qr1.failures().empty());

    // Holding the results of a call to qcheck.
    property<int, double, grouped_by<int> > const prop2 =
        _1>0 && _1%10 == 9 >>=
            group_by(_1 % 2) |
                ((_1+_2) == (_2+_1));
    auto config = make_config(_1 = igen, _2 = dgen);
    auto res = qcheck(prop2, config);
    BOOST_CHECK(res);
    BOOST_CHECK(res.success());
    BOOST_CHECK(res.exhausted());
    BOOST_CHECK(res.failures().empty());

    static_assert(
        std::is_same<
            decltype(res)
          , qcheck_results<int, double, grouped_by<int> >
        >::value,
        "qcheck returned unspected type"
    );

    std::stringstream sout;
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "Arguments exhausted after 58 tests.\n"
        "100% 1.\n"
      , sout.str()
    );

    auto config2 = make_config(_1 = igen, _2 = dgen, _max_test_count = 10000u);
    res = qcheck(prop2, config2);
    BOOST_CHECK(res);
    BOOST_CHECK(res.success());
    BOOST_CHECK(!res.exhausted());
    BOOST_CHECK(res.failures().empty());
    sout.str(std::string());
    res.print_summary(sout);
    BOOST_CHECK_EQUAL(
        "OK, passed 100 tests.\n"
        "100% 1.\n"
      , sout.str()
    );
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the qcheck algorithm");

    test->add(BOOST_TEST_CASE(&test_qcheck_results_0));

    return test;
}
