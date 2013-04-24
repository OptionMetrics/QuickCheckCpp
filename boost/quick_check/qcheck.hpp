////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QCHECK_HPP_INCLUDED
#define QCHK_QCHECK_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename Property, typename Config>
    qcheck_results<typename Config::args_type>
    qcheck(Property const &prop, Config &config)
    {
        qcheck_results<typename Config::args_type> results;

        for(std::size_t n = 0; n < config.test_count(); ++n)
        {
            auto args = config.gen();
            if(!fusion::invoke_function_object(prop, args))
                results.failures.push_back(args);
        }

        return results;
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
