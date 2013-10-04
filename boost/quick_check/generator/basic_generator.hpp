///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file basic_generator.hpp
/// \brief Header that defines some shared utilities for defining generators.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_BASIC_GENERATOR_HPP_INCLUDED
#define QCHK_GENERATOR_BASIC_GENERATOR_HPP_INCLUDED

#include <cstddef>
#include <boost/phoenix/core/argument.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<int N>
        void set_size_adl(phoenix::argument<N>, std::size_t)
        {}

        struct set_size
        {
            typedef void result_type;

            set_size(std::size_t size)
              : size_(size)
            {}

            template<typename T>
            void operator()(T &t) const
            {
                // Unqualified for ADL
                set_size_adl(t, this->size_);
            }

        private:
            std::size_t size_;
        };
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
