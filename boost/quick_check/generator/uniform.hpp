///////////////////////////////////////////////////////////////////////////////////////////////////
// \file uniform.hpp
// \brief Definition uniform, a generator that produces a uniform distribution
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_UNIFORM_HPP_INCLUDED
#define QCHK_GENERATOR_UNIFORM_HPP_INCLUDED

#include <algorithm>
#include <boost/mpl/if.hpp>
#include <boost/quick_check/detail/array.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename Value = double>
    struct uniform
      : private mpl::if_<
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

        using base_type::result_type;
        using base_type::operator();

        friend void set_size_adl(uniform &, std::size_t)
        {}
    };

    template<typename Value, std::size_t N>
    struct uniform<Value[N]>
    {
        uniform()
          : gen_()
        {}

        explicit uniform(Value a)
          : gen_(a)
        {}

        uniform(Value a, Value b)
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

        friend void set_size_adl(uniform &, std::size_t)
        {}

    private:
        uniform<Value> gen_;
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
