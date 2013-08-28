///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file normal.hpp
/// \brief Definition of \c normal<>, a generator that produces a normal distribution of floating
///        point values.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
    namespace detail
    {
        template<typename Value>
        struct normal_base
        {
            typedef boost::random::normal_distribution<Value> type;
        };
    }

    /// \brief For generating a normal distribution of floating point numbers
    ///
    /// \tparam Value The type of the values to generate. Must be a floating
    /// point type.
    ///
    /// Use \c normal<> to generate a normal distribution of floating point
    /// numbers.
    ///
    /// \em Example:
    ///
    /// \code
    /// boost::random::mt11213b rng;
    /// normal<double> one(0.0, 1.0);
    /// double d = one(rng); // generate a random double.
    /// \endcode
    template<typename Value = double>
    struct normal
      : private detail::normal_base<Value>::type
    {
    private:
        static_assert(boost::is_floating_point<Value>::value,
                      "The normal generator only works on floating-point types");

        typedef typename
            boost::random::normal_distribution<Value>
        base_type;

    public:
        /// Construct a \c normal<> object
        ///
        /// \param mean The mean of the normal distribution. Defaults to 0.0.
        /// \param std_dev The standard deviation of the normal distribution. Defaults to 1.0.
        /// \pre \c std_dev \> 0.0
        normal(Value mean, Value std_dev)
          : base_type(mean, std_dev)
        {}

        /// \overload
        explicit normal(Value mean)
          : base_type(mean)
        {}

        /// \overload
        normal()
          : base_type()
        {}

        typedef Value result_type;

        /// Generate a random value of type \c Value using \c rng as a source
        /// of randomness. The distribution of the values returned is normal,
        /// with a mean and standard deviation as specified when constructing
        /// \c *this.
        template<typename Rng>
        result_type operator()(Rng &rng)
        {
            return this->base_type::operator()(rng);
        }

        friend void set_size_adl(normal &, std::size_t)
        {}
    };

    template<typename Value, std::size_t N>
    struct normal<Value[N]>
    {
        normal(Value mean, Value std_dev)
          : gen_(mean, std_dev)
        {}

        explicit normal(Value mean)
          : gen_(mean)
        {}

        normal()
          : gen_()
        {}

        typedef detail::array<Value[N]> result_type;

        template<typename Rng>
        result_type operator()(Rng& rng)
        {
            result_type res;
            std::generate_n(res.elems.elems, N, [&] { return gen_(rng); });
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
