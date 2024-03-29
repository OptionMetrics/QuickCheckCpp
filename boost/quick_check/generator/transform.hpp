///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
/// \brief Definition of \c quick_check::transform(), for mutating the output of a generator.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_TRANSFORM_HPP_INCLUDED
#define QCHK_GENERATOR_TRANSFORM_HPP_INCLUDED

#include <boost/utility/result_of.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Generator, typename UnaryFunction>
        struct transform_generator
        {
            typedef
                typename boost::result_of<
                    UnaryFunction(typename Generator::result_type)
                >::type
            result_type;

            transform_generator(Generator const &gen, UnaryFunction fun)
              : gen_(gen)
              , fun_(fun)
            {}

            template<typename Rng>
            result_type operator()(Rng &rng)
            {
                return fun_(gen_(rng));
            }

            friend void set_size(transform_generator &thiz, std::size_t size)
            {
                detail::set_size_adl(thiz.gen_, size);
            }

        private:
            Generator gen_;
            UnaryFunction fun_;
        };
    }

    /// \brief Defines a composite generator that changes the output of an
    ///        underlying transform by running each value through the specified
    ///        UnaryFunction.
    ///
    /// \param gen The generator used to produce the elements of the sequence.
    /// \param fun The UnaryFunction used to map the input sequence to the output
    ///            sequence.
    ///
    /// If \c gen produces the sequence <tt>[a,b,c...]</tt>, then the generator
    /// returned by \c transform(gen,fun) produces the sequence
    /// <tt>[fun(a),fun(b),fun(c)...]</tt>.
    ///
    /// \pre \c fun must be an object such that 
    template<typename Generator, typename UnaryFunction>
    detail::transform_generator<Generator, UnaryFunction>
    transform(Generator const &gen, UnaryFunction fun)
    {
        return detail::transform_generator<Generator, UnaryFunction>(gen, fun);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
