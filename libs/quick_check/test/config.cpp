////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file config.cpp
/// \brief A test of make_config
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/quick_check/quick_check.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "./file_dist.hpp"

namespace qchk = boost::quick_check;

void test_config_0()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config();
    BOOST_CHECK_EQUAL(config.test_count(), 100u);
    BOOST_CHECK_EQUAL(config.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config.sized(), 50u);

    boost::fusion::vector0<> args = config();
    (void)args;
}

void test_config_1()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");

    auto config = make_config(_1 = di);
    BOOST_CHECK_EQUAL(config.test_count(), 100u);
    BOOST_CHECK_EQUAL(config.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config.sized(), 50u);

    boost::fusion::vector1<int> args = config();
    BOOST_CHECK_EQUAL(boost::fusion::at_c<0>(args), 1866415935);
}

void test_config_2()
{
    using namespace qchk;
    file_dist<int> di("uniform_int_distribution.txt");
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = di, _2 = dd);
    BOOST_CHECK_EQUAL(config.test_count(), 100u);
    BOOST_CHECK_EQUAL(config.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config.sized(), 50u);

    boost::fusion::vector2<int, double> args = config();
    BOOST_CHECK_EQUAL(boost::fusion::at_c<0>(args), 1866415935);
    BOOST_CHECK_CLOSE(boost::fusion::at_c<1>(args), 1.02379, 0.0001);
}

void test_config_3()
{
    using namespace qchk;
    file_dist<double> dd("normal_double_distribution.txt");

    auto config = make_config(_1 = _2 = dd);
    BOOST_CHECK_EQUAL(config.test_count(), 100u);
    BOOST_CHECK_EQUAL(config.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config.sized(), 50u);

    double const a = 1.02379;
    double const b = 0.176136;

    boost::fusion::vector2<double, double> args = config();
    // not sure what order they'll come out, but they'll be different
    if(boost::fusion::at_c<0>(args) > 1.0)
    {
        BOOST_CHECK_CLOSE(boost::fusion::at_c<0>(args), a, 0.0001);
        BOOST_CHECK_CLOSE(boost::fusion::at_c<1>(args), b, 0.0001);
    }
    else
    {
        BOOST_CHECK_CLOSE(boost::fusion::at_c<0>(args), b, 0.0001);
        BOOST_CHECK_CLOSE(boost::fusion::at_c<1>(args), a, 0.0001);
    }
}

void test_config_4()
{
    using namespace qchk;
    file_dist<double> dd("normal_double_distribution.txt");
    file_dist<int> di("uniform_int_distribution.txt");

    auto config = make_config(_1 = _2 = dd, _3 = di);
    BOOST_CHECK_EQUAL(config.test_count(), 100u);
    BOOST_CHECK_EQUAL(config.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config.sized(), 50u);

    double const a = 1.02379;
    double const b = 0.176136;
    int const c = 1866415935;

    boost::fusion::vector3<double, double, int> args = config();
    // not sure what order they'll come out, but they'll be different
    if(boost::fusion::at_c<0>(args) > 1.0)
    {
        BOOST_CHECK_CLOSE(boost::fusion::at_c<0>(args), a, 0.0001);
        BOOST_CHECK_CLOSE(boost::fusion::at_c<1>(args), b, 0.0001);
    }
    else
    {
        BOOST_CHECK_CLOSE(boost::fusion::at_c<0>(args), b, 0.0001);
        BOOST_CHECK_CLOSE(boost::fusion::at_c<1>(args), a, 0.0001);
    }

    BOOST_CHECK_EQUAL(boost::fusion::at_c<2>(args), c);
}

void test_config_5()
{
    using namespace qchk;
    file_dist<double> dd("normal_double_distribution.txt");
    file_dist<int> di("uniform_int_distribution.txt");

    auto config0 = make_config(_1 = dd, _2 = di, _test_count = 1001);
    BOOST_CHECK_EQUAL(config0.test_count(), 1001u);
    BOOST_CHECK_EQUAL(config0.max_test_count(), 1001u);
    BOOST_CHECK_EQUAL(config0.sized(), 50u);

    auto config1 = make_config(_1 = dd, _2 = di, _max_test_count = 1001);
    BOOST_CHECK_EQUAL(config1.test_count(), 100u);
    BOOST_CHECK_EQUAL(config1.max_test_count(), 1001u);
    BOOST_CHECK_EQUAL(config1.sized(), 50u);

    auto config2 = make_config(_1 = dd, _2 = di, _sized = 51);
    BOOST_CHECK_EQUAL(config2.test_count(), 100u);
    BOOST_CHECK_EQUAL(config2.max_test_count(), 1000u);
    BOOST_CHECK_EQUAL(config2.sized(), 51u);

    auto config3 = make_config(_1 = dd, _2 = di, _test_count = 1001, _sized = 51);
    BOOST_CHECK_EQUAL(config3.test_count(), 1001u);
    BOOST_CHECK_EQUAL(config3.max_test_count(), 1001u);
    BOOST_CHECK_EQUAL(config3.sized(), 51u);
}

struct rng_t
{};

struct int_t
{
    typedef int result_type;

    int operator()(rng_t &) const
    {
        return 42;
    }

    friend void set_size_adl(int_t &, std::size_t)
    {}
};

void test_config_6()
{
    using namespace qchk;
    rng_t rng;
    int_t di;

    auto config = make_config(_1 = di, _rng = rng);

    boost::fusion::vector1<int> args = config();

    BOOST_CHECK_EQUAL(boost::fusion::at_c<0>(args), 42);
}

using namespace boost::unit_test;
////////////////////////////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("test for make_config");

    test->add(BOOST_TEST_CASE(&test_config_0));
    test->add(BOOST_TEST_CASE(&test_config_1));
    test->add(BOOST_TEST_CASE(&test_config_2));
    test->add(BOOST_TEST_CASE(&test_config_3));
    test->add(BOOST_TEST_CASE(&test_config_4));
    test->add(BOOST_TEST_CASE(&test_config_5));
    test->add(BOOST_TEST_CASE(&test_config_6));

    return test;
}
