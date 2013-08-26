///////////////////////////////////////////////////////////////////////////////////////////////////
// \file vector.hpp
// \brief Definition of vector, for generating vectors.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_VECTOR_HPP_INCLUDED
#define QCHK_GENERATOR_VECTOR_HPP_INCLUDED

#include <vector>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    template<typename Gen>
    detail::sequence_generator<std::vector<typename Gen::result_type>, Gen, true>
    vector(Gen const &gen)
    {
        typedef std::vector<typename Gen::result_type> sequence_type;
        return detail::sequence_generator<sequence_type, Gen, true>(gen);
    }

    template<typename Gen>
    detail::sequence_generator<std::vector<typename Gen::result_type>, Gen, true, true>
    ordered_vector(Gen const &gen)
    {
        typedef std::vector<typename Gen::result_type> sequence_type;
        return detail::sequence_generator<sequence_type, Gen, true, true>(gen);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
