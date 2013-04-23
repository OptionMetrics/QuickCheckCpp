////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_QUICK_CHECK_PROPERTY_HPP_INCLUDED
#define BOOST_QUICK_CHECK_PROPERTY_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/qcheck_results.hpp>

QKCK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename A, typename B /*... and more*/>
    struct property
    {
    public:
        typedef qcheck_results<A, B> qcheck_results_type;
        typedef fusion::vector<A, B> args_type;

        template<typename Actor>
        property(phoenix::actor<Actor> const &actor)
          : pred_(actor)
        {}

        typedef bool result_type; // for TR1
        bool operator()(A const &a, B const &b) const
        {
            return this->pred_(a, b);
        }

    private:
        boost::function<bool(A, B)> pred_;
    };
}

QKCK_BOOST_NAMESPACE_END

#endif
