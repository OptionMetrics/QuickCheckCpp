////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file string_gen.cpp
/// \brief A test of the string generator
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
#include <iostream>
#include <iomanip>
#include <functional>
#include <boost/quick_check/quick_check.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/test/unit_test.hpp>
#include "../file_dist.hpp"

static const std::size_t CLOOPS = 128;

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

BOOST_TEST_DONT_PRINT_LOG_VALUE( std::string::iterator );

template<typename Gen, typename Pred>
void test_string_with_char_gen(Gen gen, Pred pred)
{
    for (int i = 0; i < CLOOPS; ++i)
    {
        std::string str = gen(rng);
        auto it = boost::find_if(str, std::not1(pred));
        if(it != str.end())
            BOOST_ERROR("Invalid character generated : " << *it << " (" <<
                        std::hex << std::showbase << (unsigned int)*it << ")");
    }
}

void test_string_with_char()
{
    test_string_with_char_gen(qchk::string(qchk::alnum()), ::isalnum_fun());
    test_string_with_char_gen(qchk::string(qchk::alpha()), ::isalpha_fun());
    test_string_with_char_gen(qchk::string(qchk::cntrl()), ::iscntrl_fun());
    test_string_with_char_gen(qchk::string(qchk::digit()), ::isdigit_fun());
    test_string_with_char_gen(qchk::string(qchk::graph()), ::isgraph_fun());
    test_string_with_char_gen(qchk::string(qchk::lower()), ::islower_fun());
    test_string_with_char_gen(qchk::string(qchk::print()), ::isprint_fun());
    test_string_with_char_gen(qchk::string(qchk::punct()), ::ispunct_fun());
    test_string_with_char_gen(qchk::string(qchk::space()), ::isspace_fun());
    test_string_with_char_gen(qchk::string(qchk::upper()), ::isupper_fun());
    test_string_with_char_gen(qchk::string(qchk::xdigit()), ::isxdigit_fun());
}

void test_string()
{
    auto sgen = qchk::string();
    set_size_adl(sgen, 2u);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::string str = sgen(rng);
        BOOST_CHECK_LE(str.length(), 2u);
    }

    set_size_adl(sgen, 64u);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::string str = sgen(rng);
        BOOST_CHECK_LE(str.length(), 64u);
    }
}

void test_wstring()
{
    auto sgen = qchk::wstring();
    set_size_adl(sgen, 2u);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::wstring str = sgen(rng);
        BOOST_CHECK_LE(str.length(), 2u);
    }

    set_size_adl(sgen, 64u);

    for(std::size_t i = 0; i < CLOOPS; ++i)
    {
        std::wstring str = sgen(rng);
        BOOST_CHECK_LE(str.length(), 64u);
    }
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("tests for the string generators");

    test->add(BOOST_TEST_CASE(&test_string));
    test->add(BOOST_TEST_CASE(&test_wstring));
    test->add(BOOST_TEST_CASE(&test_string_with_char));

    return test;
}
