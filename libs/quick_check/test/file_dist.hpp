///////////////////////////////////////////////////////////////////////////////////////////////////
// \file file_dist.hpp
// \brief A "distribution" that pulls random numbers from an input file.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_FILE_DIST_INCLUDED
#define QCHK_FILE_DIST_INCLUDED

#include <memory>
#include <fstream>

template<typename Value>
struct file_dist
{
    explicit file_dist(std::string const &file)
      : pfin_(std::make_shared<std::ifstream>(file))
    {
        if(!*pfin_)
            throw std::runtime_error(std::string("Cannot find input file: ") + file);
    }

    template<typename Gen>
    Value operator()(Gen &)
    {
        Value val;
        if(!(*pfin_ >> val))
            throw std::runtime_error("Random numbers exhausted");
        return val;
    }

    friend void set_size_adl(file_dist &, std::size_t)
    {}

private:
    std::shared_ptr<std::ifstream> pfin_;
};

#endif
