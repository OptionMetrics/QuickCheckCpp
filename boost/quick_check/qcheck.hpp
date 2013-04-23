////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_QUICK_CHECK_QCHECK_HPP_INCLUDED
#define BOOST_QUICK_CHECK_QCHECK_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>

QKCK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename Property, typename Config>
    typename Property::qcheck_results_type
    qcheck(Property const &prop, Config &config)
    {
        typename Property::qcheck_results_type results;

        for(std::size_t n = 0; n < config.test_count(); ++n)
        {
            typename Property::args_type args =
                fusion::make_vector(config.get_int(), config.get_int());
            if(!prop(fusion::at_c<0>(args), fusion::at_c<1>(args)))
                results.failures_.push_back(args);
        }

        return results;
    }
}

QKCK_BOOST_NAMESPACE_END

#endif
