///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file uniform.hpp
/// \brief Definition of \c quick_check::uniform\<\>, a generator that produces a uniform
///        distribution of floating point or integral values.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
#include <boost/type_traits/is_integral.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Value>
        struct uniform_base
          : mpl::if_<
                boost::is_floating_point<Value>
              , boost::random::uniform_real_distribution<Value>
              , boost::random::uniform_int_distribution<Value>
            >
        {};
    }

    /// \brief A generator for a sequence of uniformly distributed integral or
    ///        floating point numbers.
    ///
    /// \tparam Value The type of the generated values. It must be an integral
    ///               or floating point type.
    ///
    /// Use \c uniform\<\> to generate a uniform distribution of integral or
    /// floating point numbers between some minimum and maximum.
    ///
    /// \em Example:
    ///
    /// \code
    /// boost::random::mt11213b rng;
    /// uniform<int> die(1, 6);
    /// // generate a random int between 1 and 6 inclusive
    /// int i = die(rng);
    /// \endcode
    template<typename Value = double>
    struct uniform
      : private detail::uniform_base<Value>::type
    {
    private:
        static_assert(boost::is_floating_point<Value>::value ||
                      boost::is_integral<Value>::value,
                      "The uniform generator only works on floating-point or "
                      "integral types");

        typedef typename detail::uniform_base<Value>::type base_type;

    public:
        /// \brief Construct a \c uniform\<\> object
        /// \param min The minimum value (inclusive) of the resulting sequence.
        ///            \c min defaults to 0.
        /// \param max The maximum value (inclusive) of the resulting sequence.
        ///            \c max defaults to \c std::numeric_limits\<Value\>::max() if
        ///            \c Value is an integral type; otherwise, it defaults to 1.0.
        /// \pre \c min \< \c max
        /// \remark Note that the minimum and maximum values of the sequence
        ///         are both inclusive. Contrary to common C++ idiom, they denote
        ///         a closed range of values, not a half-open one.
        uniform(Value min, Value max)
          : base_type(min, max)
        {}

        /// \overload
        ///
        explicit uniform(Value min)
          : base_type(min)
        {}

        /// \overload
        ///
        uniform()
          : base_type()
        {}

        typedef Value result_type;

        /// \param rng A random number generator
        ///
        /// Generate a random value of type \c Value using \c rng as a source
        /// of randomness. The distribution of the values returned is unifom,
        /// with a minimum and maximum (inclusive) as specified when constructing
        /// \c *this.
        template<typename Rng>
        result_type operator()(Rng &rng)
        {
            return this->base_type::operator()(rng);
        }

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
            std::generate_n(res.elems.elems, N, [&] { return gen_(rng); });
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
