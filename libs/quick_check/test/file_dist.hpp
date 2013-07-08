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
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/quick_check/detail/array.hpp>

template<typename Value>
struct file_dist
{
    typedef Value result_type;

    explicit file_dist(std::string const &file)
      : pfin_(boost::make_shared<std::ifstream>(file))
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
    boost::shared_ptr<std::ifstream> pfin_;
};

template<typename Value, std::size_t N>
struct file_dist<Value[N]>
{
    typedef boost::quick_check::detail::array<Value[N]> result_type;

    explicit file_dist(std::string const &file)
      : dist_(file)
    {}

    template<typename Gen>
    boost::quick_check::detail::array<Value[N]> operator()(Gen &g)
    {
        boost::quick_check::detail::array<Value[N]> res;
        std::generate_n(res.elems.elems, N, [&](){return dist_(g);});
        return res;
    }

    friend void set_size_adl(file_dist &, std::size_t)
    {}

private:
    file_dist<Value> dist_;
};

#endif
