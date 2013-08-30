///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file vector.hpp
/// \brief Definition of \c quick_check::vector(), for generating vectors.
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
    /// \brief Create a generator for \c std::vector\<\> objects.
    ///
    /// \param gen The generator used to generate the elements fot the vector.
    ///
    /// The element type of the resulting vectors is the return type of
    /// \c gen's overloaded function call operator.
    ///
    /// The size of the resulting vectors is controlled by the \c quick_check::_sized
    /// named parameter to the \c quick_check::make_config() function, or by calling
    /// \c quick_check::config::sized() on the \c config\<\> object that
    /// \c quick_check::make_config() returns.
    ///
    /// \sa \c quick_check::ordered_vector
    /// \sa \c quick_check::_sized
    /// \sa \c quick_check::config::sized
    template<typename Gen>
    detail::sequence_generator<std::vector<typename Gen::result_type>, Gen, true>
    vector(Gen const &gen)
    {
        typedef std::vector<typename Gen::result_type> sequence_type;
        return detail::sequence_generator<sequence_type, Gen, true>(gen);
    }

    /// \brief Create a generator for sorted \c std::vector\<\> objects.
    ///
    /// \param gen The generator used to generate the elements fot the vector.
    ///
    /// \pre \c Gen::result_type is a type which has a partial order over the
    ///      \< operator.
    ///
    /// The element type of the resulting vectors is the return type of
    /// \c gen's overloaded function call operator.
    ///
    /// The resulting vectors are guaranteed to be sorted.
    ///
    /// The size of the resulting vectors is controlled by the \c quick_check::_sized
    /// named parameter to the \c quick_check::make_config() function, or by calling
    /// \c quick_check::config::sized() on the \c config\<\> object that
    /// \c quick_check::make_config() returns.
    ///
    /// \sa \c quick_check::vector
    /// \sa \c quick_check::_sized
    /// \sa \c quick_check::config::sized
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
