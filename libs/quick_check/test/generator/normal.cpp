////////////////////////////////////////////////////////////////////////////////////////////////////
// \file normal_gen.cpp
// \brief A test for the constant generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>

namespace qchk = boost::quick_check;

// Can't really test the distribution's properties, but can at least check that
// things compile that should.
void test_normal()
{
    boost::random::mt11213b rng;

    qchk::normal<float> nf(0.,1.);
    boost::random::normal_distribution<float> brnf(0.,1.); 
    float f = nf(rng);
    BOOST_CHECK_GE(f, brnf.min());
    BOOST_CHECK_LE(f, brnf.max());

    qchk::normal<double> nd(0.,1.);
    boost::random::normal_distribution<double> brnd(0.,1.); 
    double d = nd(rng);
    BOOST_CHECK_GE(d, brnd.min());
    BOOST_CHECK_LE(d, brnd.max());

    qchk::normal<long double> nld(0.,1.);
    boost::random::normal_distribution<long double> brnld(0.,1.); 
    long double ld = nld(rng);
    BOOST_CHECK_GE(ld, brnld.min());
    BOOST_CHECK_LE(ld, brnld.max());

    qchk::normal<float[3]> anf(0.,1.);
    qchk::detail::array<float[3]> af = anf(rng);
    BOOST_CHECK_GE(af.elems[0], brnf.min());
    BOOST_CHECK_LE(af.elems[0], brnf.max());

    qchk::normal<double[3]> _and(0.,1.);
    qchk::detail::array<double[3]> ad = _and(rng);
    BOOST_CHECK_GE(ad.elems[0], brnd.min());
    BOOST_CHECK_LE(ad.elems[0], brnd.max());

    qchk::normal<long double[3]> anld(0.,1.);
    qchk::detail::array<long double[3]> ald = anld(rng);
    BOOST_CHECK_GE(ald.elems[0], brnld.min());
    BOOST_CHECK_LE(ald.elems[0], brnld.max());
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("basic tests for normal generator");

    test->add(BOOST_TEST_CASE(&test_normal));

    return test;
}
