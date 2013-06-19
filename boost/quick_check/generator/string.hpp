///////////////////////////////////////////////////////////////////////////////////////////////////
// \file string.hpp
// \brief Definition of string, for generating strings.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
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
    template<typename Generator>
    detail::sequence_generator<std::basic_string<typename Generator::result_type>, Generator, true>
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

    inline detail::sequence_generator<std::string, uniform<char>, true>
    string()
    {
        typedef detail::sequence_generator<std::string, uniform<char>, true> result_type;
        return result_type(uniform<char>());
    }

    inline detail::sequence_generator<std::wstring, uniform<wchar_t>, true>
    wstring()
    {
        typedef detail::sequence_generator<std::wstring, uniform<wchar_t>, true> result_type;
        return result_type(uniform<wchar_t>());
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
