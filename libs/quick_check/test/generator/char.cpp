////////////////////////////////////////////////////////////////////////////////////////////////////
// \file char.cpp
// \brief A test of the char generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

namespace qchk = boost::quick_check;

#define DEFINE_CCTYPE_PRED_TYPE(ISCTYPE)                    \
struct ISCTYPE ## _fun : std::unary_function<char, bool>    \
{                                                           \
    bool operator()(char c) const                           \
    {                                                       \
        return 0 != std::ISCTYPE((int)(unsigned char)c);    \
    }                                                       \
};                                                          \
/**/

DEFINE_CCTYPE_PRED_TYPE(isalnum)
DEFINE_CCTYPE_PRED_TYPE(isalpha)
DEFINE_CCTYPE_PRED_TYPE(iscntrl)
DEFINE_CCTYPE_PRED_TYPE(isdigit)
DEFINE_CCTYPE_PRED_TYPE(isgraph)
DEFINE_CCTYPE_PRED_TYPE(islower)
DEFINE_CCTYPE_PRED_TYPE(isprint)
DEFINE_CCTYPE_PRED_TYPE(ispunct)
DEFINE_CCTYPE_PRED_TYPE(isspace)
DEFINE_CCTYPE_PRED_TYPE(isupper)
DEFINE_CCTYPE_PRED_TYPE(isxdigit)

// A source for random numbers
boost::random::mt11213b rng;

template<typename Gen, typename Pred>
void test_char_gen(Gen gen, Pred pred)
{
    for (int i = 0; i < 10000; ++i)
    {
        char ch = gen(rng);
        BOOST_CHECK(pred(ch));
    }
}

void test_char()
{
    test_char_gen(qchk::alnum(), ::isalnum_fun());
    test_char_gen(qchk::alpha(), ::isalpha_fun());
    test_char_gen(qchk::cntrl(), ::iscntrl_fun());
    test_char_gen(qchk::digit(), ::isdigit_fun());
    test_char_gen(qchk::graph(), ::isgraph_fun());
    test_char_gen(qchk::lower(), ::islower_fun());
    test_char_gen(qchk::print(), ::isprint_fun());
    test_char_gen(qchk::punct(), ::ispunct_fun());
    test_char_gen(qchk::space(), ::isspace_fun());
    test_char_gen(qchk::upper(), ::isupper_fun());
    test_char_gen(qchk::xdigit(), ::isxdigit_fun());
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the char generators");

    test->add(BOOST_TEST_CASE(&test_char));

    return test;
}
