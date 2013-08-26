///////////////////////////////////////////////////////////////////////////////////////////////////
// \file constant.hpp
// \brief Definition constant, a generator that always produces the same value
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

    template<typename T>
    detail::constant_generator<T> constant(T const &t)
    {
        return detail::constant_generator<T>(t);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
