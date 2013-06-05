///////////////////////////////////////////////////////////////////////////////////////////////////
// \file generator.hpp
// \brief Definition of some generic generators
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_HPP_INCLUDED
#define QCHK_GENERATOR_HPP_INCLUDED

#include <algorithm>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/detail/array.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/mpl/if.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/fusion/view/transform_view.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Type>
        struct construct
        {
            typedef Type result_type;

            Type operator()() const
            {
                return Type();
            }

#define BOOST_PP_LOCAL_MACRO(N)                                                                     \
            template<BOOST_PP_ENUM_PARAMS(N, typename T)>                                           \
            Type operator()(BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &t)) const                      \
            {                                                                                       \
                return Type(BOOST_PP_ENUM_PARAMS(N, t));                                            \
            }                                                                                       \
            /**/
#define BOOST_PP_LOCAL_LIMITS (1, QCHK_MAX_ARITY)
#include BOOST_PP_LOCAL_ITERATE()
        };

        template<typename Rng>
        struct gen_fun
        {
            explicit gen_fun(Rng &rng)
              : rng_(rng)
            {}

            template<typename Sig>
            struct result;

            template<typename This, typename Gen>
            struct result<This(Gen &)>
              : boost::result_of<unpack_array(typename boost::result_of<Gen(Rng &)>::type)>
            {};

            template<typename Gen>
            typename boost::result_of<unpack_array(typename boost::result_of<Gen(Rng &)>::type)>::type
            operator()(Gen &gen) const
            {
                return unpack_array()(gen(rng_));
            }

            Rng &rng_;
        };

        template<typename Type, typename Generators>
        struct custom_generator
        {
            typedef Type result_type;

            explicit custom_generator(Generators const &gens)
              : gens_(gens)
            {}

            template<typename Rng>
            Type operator()(Rng& rng)
            {
                detail::gen_fun<Rng> gen(rng);
                return fusion::invoke_function_object(
                    detail::construct<Type>()
                  , fusion::transform_view<Generators, detail::gen_fun<Rng> >(gens_, gen)
                );
            }

        private:
            Generators gens_;
        };
    }

    template<typename Value = double>
    struct uniform
      : private mpl::if_<
            boost::is_floating_point<Value>
          , boost::random::uniform_real_distribution<Value>
          , boost::random::uniform_int_distribution<Value>
        >::type
    {
        typedef typename
            mpl::if_<
                boost::is_floating_point<Value>
              , boost::random::uniform_real_distribution<Value>
              , boost::random::uniform_int_distribution<Value>
            >::type
        base_type;

        uniform()
          : base_type()
        {}

        explicit uniform(Value a)
          : base_type(a)
        {}

        uniform(Value a, Value b)
          : base_type(a, b)
        {}

        using base_type::result_type;
        using base_type::operator();
    };

    template<typename Value, std::size_t N>
    struct uniform<Value[N]>
    {
        uniform()
          : gen_()
        {}

        explicit uniform(Value a)
          : gen_(a)
        {}

        uniform(Value a, Value b)
          : gen_(a, b)
        {}

        typedef detail::array<Value[N]> result_type;

        template<typename Rng>
        result_type operator()(Rng& rng)
        {
            result_type res;
            fusion::for_each(res.elems, [&,this](Value &val) { val = gen_(rng); });
            return res;
        }

    private:
        uniform<Value> gen_;
    };

    template<typename Value = double>
    struct normal
      : boost::random::normal_distribution<Value>
    {
        static_assert(boost::is_floating_point<Value>::value,
                      "The normal generator only works on floating-point types");

        typedef typename
            boost::random::normal_distribution<Value>
        base_type;

        normal()
          : base_type()
        {}

        explicit normal(Value a)
          : base_type(a)
        {}

        normal(Value a, Value b)
          : base_type(a, b)
        {}
    };

    template<typename Value, std::size_t N>
    struct normal<Value[N]>
    {
        normal()
          : gen_()
        {}

        explicit normal(Value a)
          : gen_(a)
        {}

        normal(Value a, Value b)
          : gen_(a, b)
        {}

        typedef detail::array<Value[N]> result_type;

        template<typename Rng>
        result_type operator()(Rng& rng)
        {
            result_type res;
            std::generate(res.elems.elems, res.elems.elems + N, [&] { return gen_(rng); });
            return res;
        }

    private:
        normal<Value> gen_;
    };

    /// \brief Create a custom generator for a type given several other generators to use
    ///        for generating constructor arguments.
#define BOOST_PP_LOCAL_MACRO(N)                                                                     \
    template<typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, typename G)>                               \
    detail::custom_generator<                                                                       \
        T                                                                                           \
      , typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type                   \
    >                                                                                               \
    make_gen(BOOST_PP_ENUM_BINARY_PARAMS(N, const G, &g))                                           \
    {                                                                                               \
        return detail::custom_generator<                                                            \
            T                                                                                       \
          , typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type               \
        >(fusion::make_vector(BOOST_PP_ENUM_PARAMS(N, g)));                                         \
    }                                                                                               \
    /**/
#define BOOST_PP_LOCAL_LIMITS (1, QCHK_MAX_ARITY)
#include BOOST_PP_LOCAL_ITERATE()

}

QCHK_BOOST_NAMESPACE_END

#endif
