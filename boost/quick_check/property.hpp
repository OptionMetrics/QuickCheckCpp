///////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_PROPERTY_HPP_INCLUDED
#define QCHK_PROPERTY_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/qcheck_results.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename A, typename B /*... and more*/>
    struct property
    {
    public:
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

QCHK_BOOST_NAMESPACE_END

#endif
