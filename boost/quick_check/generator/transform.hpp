///////////////////////////////////////////////////////////////////////////////////////////////////
// \file transform.hpp
// \brief Definition of transform, for mutating the output of a generator
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
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

            friend void set_size_adl(transform_generator &thiz, std::size_t size)
            {
                set_size_adl(thiz.gen_);
            }

        private:
            Generator gen_;
            UnaryFunction fun_;
        };
    }

    template<typename Generator, typename UnaryFunction>
    detail::transform_generator<Generator, UnaryFunction>
    transform(Generator const &gen, UnaryFunction fun)
    {
        return detail::transform_generator<Generator, UnaryFunction>(gen, fun);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
