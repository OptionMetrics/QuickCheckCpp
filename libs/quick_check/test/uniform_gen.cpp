////////////////////////////////////////////////////////////////////////////////////////////////////
// \file uniform_gen.cpp
// \brief A test for the constant generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/config.hpp>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>

#ifdef BOOST_CLANG
# define QCHK_CLANG (__clang_major__ * 100 + __clang_minor__)
#endif

namespace qchk = boost::quick_check;

boost::random::mt11213b rng;

template<typename Integral>
void test_uniform_integral(Integral min, Integral max)
{
    qchk::uniform<Integral> ui(min,max);
    Integral i = ui(rng);
    BOOST_CHECK_GE(i, min);
    BOOST_CHECK_LE(i, max);

    qchk::uniform<Integral[3]> aui(min, max);
    qchk::detail::array<Integral[3]> ai = aui(rng);
    BOOST_CHECK_GE(ai[0], min);
    BOOST_CHECK_LE(ai[0], max);
}

template<typename Floating>
void test_uniform_floating(Floating min, Floating max)
{
    qchk::uniform<Floating> uf(min,max);
    Floating f = uf(rng);
    BOOST_CHECK_GE(f, min);
    BOOST_CHECK_LE(f, max);

    qchk::uniform<Floating[3]> auf(min, max);
    qchk::detail::array<Floating[3]> af = auf(rng);
    BOOST_CHECK_GE(af[0], min);
    BOOST_CHECK_LE(af[0], max);
}

void test_uniform()
{
    test_uniform_integral<char>(0, 100);
    test_uniform_integral<signed char>(-100, 100);
    test_uniform_integral<unsigned char>(0, 100);
    test_uniform_integral<wchar_t>(0, 100);
    test_uniform_integral<short>(-100, 100);
    test_uniform_integral<unsigned short>(0u, 100u);
    test_uniform_integral<int>(-100, 100);
    test_uniform_integral<unsigned int>(0u, 100u);
    test_uniform_integral<long>(-100, 100);
    test_uniform_integral<unsigned long>(0u, 100u);

#ifndef BOOST_NO_LONG_LONG
    test_uniform_integral<long long>(-100, 100);
    test_uniform_integral<unsigned long long>(0u, 100u);
#endif

    // workaround for type_trats bug: https://svn.boost.org/trac/boost/ticket/8780
#if !(BOOST_WORKAROUND(BOOST_GCC, BOOST_TESTED_AT(40801)) || \
      BOOST_WORKAROUND(QCHK_CLANG, BOOST_TESTED_AT(304)))
#ifndef BOOST_NO_CXX11_CHAR16_T
    test_uniform_integral<char16_t>(0, 100);
#endif

#ifndef BOOST_NO_CXX11_CHAR32_T
    test_uniform_integral<char32_t>(0, 100);
#endif
#endif

    test_uniform_floating<float>(-100.f,100.f);
    test_uniform_floating<double>(-100.,100.);
    test_uniform_floating<long double>(-100.,100.);
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("basic tests for uniform generator");

    test->add(BOOST_TEST_CASE(&test_uniform));

    return test;
}
