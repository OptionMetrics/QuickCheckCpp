///////////////////////////////////////////////////////////////////////////////////////////////////
// \file generator.hpp
// \brief Definition of some generic generators
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_HPP_INCLUDED
#define QCHK_GENERATOR_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/mpl/if.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename Value = double>
    struct uniform
      : mpl::if_<
            boost::is_floating_point<Value>
          , boost::random::uniform_real_distribution<Value>
          , boost::random::uniform_int_distribution<Value>
        >::type
    {
        typedef typename
            mpl::if_<
                boost::is_floating_point<Value>
              , boost::random::uniform_real_distribution<Value>
              , boost::random::uniform_int_distribution<Value>
            >::type
        base_type;

        uniform()
          : base_type()
        {}

        explicit uniform(Value a)
          : base_type(a)
        {}

        uniform(Value a, Value b)
          : base_type(a, b)
        {}
    };

    template<typename Value = double>
    struct normal
      : boost::random::normal_distribution<Value>
    {
        static_assert(boost::is_floating_point<Value>::value,
                      "The normal generator only works on floating-point types");

        typedef typename
            boost::random::normal_distribution<Value>
        base_type;

        normal()
          : base_type()
        {}

        explicit normal(Value a)
          : base_type(a)
        {}

        normal(Value a, Value b)
          : base_type(a, b)
        {}
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
