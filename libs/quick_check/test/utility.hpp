////////////////////////////////////////////////////////////////////////////////////////////////////
// \file utility.cpp
// \brief Utilities for the quick_check test suite
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_TEST_UTILITY_HPP_INCLUDED
#define QCHK_TEST_UTILITY_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace qcheck_test
{
/// Returns the absolute path to the file relative to the root
/// of the quick_check tests.
///
/// @throw runtime_error if the resulting path doesn't exist
inline std::string abs_test_root(std::string file)
{
    namespace fs = boost::filesystem;
    fs::path p(__FILE__);
    p.remove_filename();
    p /= file;
    if(!fs::exists(p))
        throw std::runtime_error("file not found : " + file);
    return p.string();
}
}

#endif
