///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file zip.hpp
/// \brief Definition of \c quick_check::zip(), for building a \c std::pair\<\> with two generators
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_ZIP_HPP_INCLUDED
#define QCHK_GENERATOR_ZIP_HPP_INCLUDED

#include <utility>
#include <boost/utility/result_of.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Generator0, typename Generator1>
        struct zip_generator
        {
            typedef
                std::pair<
                    typename Generator0::result_type
                  , typename Generator1::result_type
                >
            result_type;

            zip_generator(Generator0 const &gen0, Generator1 const &gen1)
              : gen0_(gen0)
              , gen1_(gen1)
            {}

            template<typename Rng>
            result_type operator()(Rng &rng)
            {
                return result_type(gen0_(rng), gen1_(rng));
            }

            friend void set_size(zip_generator &thiz, std::size_t size)
            {
                detail::set_size_adl(thiz.gen0_, size);
                detail::set_size_adl(thiz.gen1_, size);
            }

        private:
            Generator0 gen0_;
            Generator1 gen1_;
        };
    }

    /// \brief Creates a generator for \c std::pair\<\> objects from two
    ///        generators.
    ///
    /// \pre \c gen0 and \c gen1 are valid Generator objects.
    ///
    /// \c zip() returns a Generator for \c std::pair\<\> objects. The
    /// \c std::pair\<\> objects have type
    /// <tt>std::pair\<Generator0::result_type, Generator1::result_type\></tt>.
    ///
    /// Given a random number generator \c rng, the \c std::pair\<\> objects
    /// are created as if with <tt>std::make_pair(gen0(rng), gen1(rng))</tt>.
    template<typename Generator0, typename Generator1>
    detail::zip_generator<Generator0, Generator1>
    zip(Generator0 const &gen0, Generator1 const &gen1)
    {
        return detail::zip_generator<Generator0, Generator1>(gen0, gen1);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
