////////////////////////////////////////////////////////////////////////////////////////////////////
// \file config.hpp
// \brief Definition of test configuration objects
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_QUICK_CHECK_CONFIG_HPP_INCLUDED
#define BOOST_QUICK_CHECK_CONFIG_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/random/mersenne_twister.hpp>

QKCK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    struct default_config
    {
        std::size_t test_count() const
        {
            return 100;
        }

        boost::uint64_t get_int()
        {
            return this->rng_();
        }

    private:
        boost::random::mt19937_64 rng_;
    };
}

QKCK_BOOST_NAMESPACE_END

#endif
