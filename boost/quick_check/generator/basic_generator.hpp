///////////////////////////////////////////////////////////////////////////////////////////////////
// \file class.hpp
// \brief Definition class_gen, for generating objects of user-defined types.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
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
