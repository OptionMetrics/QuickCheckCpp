///////////////////////////////////////////////////////////////////////////////////////////////////
// \file normal.hpp
// \brief Definition of normal, a generator that produces a normal distribution
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_NORMAL_HPP_INCLUDED
#define QCHK_GENERATOR_NORMAL_HPP_INCLUDED

#include <algorithm>
#include <boost/quick_check/detail/array.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
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

        friend void set_size_adl(normal &, std::size_t)
        {}
    };

    template<typename Value, std::size_t N>
    struct normal<Value[N]>
    {
        normal()
          : gen_()
        {}

        explicit normal(Value a)
          : gen_(a)
        {}

        normal(Value a, Value b)
          : gen_(a, b)
        {}

        typedef detail::array<Value[N]> result_type;

        template<typename Rng>
        result_type operator()(Rng& rng)
        {
            result_type res;
            std::generate(res.elems.elems, res.elems.elems + N, [&] { return gen_(rng); });
            return res;
        }

        friend void set_size_adl(normal &, std::size_t)
        {}

    private:
        normal<Value> gen_;
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
