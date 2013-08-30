///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file string.hpp
/// \brief Definition of \c quick_check::string() and \c quick_check::wstring(), for generating
///        strings.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_STRING_HPP_INCLUDED
#define QCHK_GENERATOR_STRING_HPP_INCLUDED

#include <string>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>
#include <boost/quick_check/generator/uniform.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    /// Create a generator for \c std::basic_string\<\> objects.
    ///
    /// \param gen The generator used to generate the characters of the string. Defaults
    ///            to \c quick_check::uniform\<char\>().
    ///
    /// The character type of the resulting strings is the return type of
    /// \c gen's overloaded function call operator.
    ///
    /// If no generator is specified, it is assumed to be \c quick_check::uniform\<char\>.
    ///
    /// \sa \c quick_check::wstring
    template<typename Generator>
    detail::sequence_generator<
        std::basic_string<typename Generator::result_type>
      , Generator
      , true
    >
    string(Generator const &gen)
    {
        typedef
            detail::sequence_generator<
                std::basic_string<typename Generator::result_type>
              , Generator
              , true
            >
        result_type;
        return result_type(gen);
    }

    /// \overload
    ///
    inline detail::sequence_generator<std::string, uniform<char>, true>
    string()
    {
        typedef detail::sequence_generator<std::string, uniform<char>, true> result_type;
        return result_type(uniform<char>());
    }

    /// Create a generator for \c std::wstring objects.
    ///
    /// \c quick_check::uniform\<wchar_t\> is used to generate the characters of the string.
    inline detail::sequence_generator<std::wstring, uniform<wchar_t>, true>
    wstring()
    {
        typedef detail::sequence_generator<std::wstring, uniform<wchar_t>, true> result_type;
        return result_type(uniform<wchar_t>());
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
