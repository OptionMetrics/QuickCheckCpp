////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck_results.hpp
// \brief Definition of qcheck_results object
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_QUICK_CHECK_QCHECK_RESULTS_HPP_INCLUDED
#define BOOST_QUICK_CHECK_QCHECK_RESULTS_HPP_INCLUDED

#include <vector>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/optional.hpp>

QKCK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename A, typename B /* extra args ...*/ >
    struct qcheck_results
    {
    public:
        qcheck_results()
          : failures_()
        {}

        bool success() const
        {
            return this->failures_.empty();
        }

    //private:
        std::vector<fusion::vector<A, B> > failures_;
    };

}

QKCK_BOOST_NAMESPACE_END

#endif
