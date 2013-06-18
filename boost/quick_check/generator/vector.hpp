///////////////////////////////////////////////////////////////////////////////////////////////////
// \file vector.hpp
// \brief Definition of vector_gen, for generating vectors.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
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
    vector_gen(Gen const &gen)
    {
        return detail::sequence_generator<std::vector<typename Gen::result_type>, Gen, true>(gen);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
