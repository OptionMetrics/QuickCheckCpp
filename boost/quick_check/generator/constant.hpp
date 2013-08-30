///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file constant.hpp
/// \brief Definition of \c quick_check::constant\<\>, a generator that always produces the same 
///        value.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_CONSTANT_HPP_INCLUDED
#define QCHK_GENERATOR_CONSTANT_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Value>
        struct constant_generator
        {
            typedef Value result_type;

            explicit constant_generator(Value const &v)
              : value_(v)
            {}

            template<typename Rng>
            Value operator()(Rng&) const
            {
                return value_;
            }

            friend void set_size_adl(constant_generator &, std::size_t)
            {}

        private:
            Value value_;
        };
    }

    /// \brief Declare a degenerate generator that always produces the same
    /// value
    ///
    /// Sometimes it's handy to have a generator that always produces the same
    /// value; for instance, when combining primitive generators into a composite
    /// generator, like \c quick_check::class_().
    ///
    /// The code below shows how to use \c constant
    /// \code
    /// auto gen = constant(42);
    ///
    /// boost::random::mt11213b rng;
    /// assert(gen(rng) == 42);
    /// \endcode
    template<typename T>
    detail::constant_generator<T> constant(T const &t)
    {
        return detail::constant_generator<T>(t);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
