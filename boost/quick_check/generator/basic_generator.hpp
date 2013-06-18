///////////////////////////////////////////////////////////////////////////////////////////////////
// \file class.hpp
// \brief Definition class_gen, for generating objects of user-defined types.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_BASIC_GENERATOR_HPP_INCLUDED
#define QCHK_GENERATOR_BASIC_GENERATOR_HPP_INCLUDED

#include <cstddef>
#include <algorithm>
#include <boost/mpl/bool.hpp>
#include <boost/phoenix/core/argument.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<int N>
        void set_size_adl(phoenix::argument<N>, std::size_t)
        {}

        struct set_size
        {
            typedef void result_type;

            set_size(std::size_t size)
              : size_(size)
            {}

            template<typename T>
            void operator()(T &t) const
            {
                // Unqualified for ADL
                set_size_adl(t, this->size_);
            }

        private:
            std::size_t size_;
        };

        template<typename Seq, typename Gen, bool Reserve = false>
        struct sequence_generator
        {
            typedef Seq result_type;

            explicit sequence_generator(Gen const &gen)
              : size_dist_(0,49)
              , gen_(gen)
            {}

            template<typename Rng>
            result_type operator()(Rng &rng)
            {
                std::size_t size = size_dist_(rng);
                result_type res;
                sequence_generator::reserve_(res, size, mpl::bool_<Reserve>());
                std::generate_n(std::back_inserter(res), size, [&] { return gen_(rng); });
                return res;
            }

            friend void set_size_adl(sequence_generator &thiz, std::size_t size)
            {
                BOOST_ASSERT(size >= 1);
                boost::random::uniform_int_distribution<std::size_t>::param_type parm(0, size - 1);
                thiz.size_dist_.param(parm);
                set_size_adl(thiz.gen_, size); // non-qualified, for ADL
            }

        private:
            static void reserve_(Seq &, std::size_t size, mpl::false_)
            {}

            static void reserve_(Seq &seq, std::size_t size, mpl::true_)
            {
                seq.reserve(size);
            }

            boost::random::uniform_int_distribution<std::size_t> size_dist_;
            Gen gen_;
        };
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
