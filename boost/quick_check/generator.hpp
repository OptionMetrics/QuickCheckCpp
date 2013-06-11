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
#include <boost/assert.hpp>
#include <boost/optional.hpp>
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
#include <boost/fusion/sequence/intrinsic/value_at.hpp>

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
            typename boost::result_of<
                unpack_array(typename boost::result_of<Gen(Rng &)>::type)
            >::type
            operator()(Gen &gen) const
            {
                return unpack_array()(gen(rng_));
            }

            Rng &rng_;
        };

        template<typename Type, typename Generators>
        struct object_generator
        {
            typedef Type result_type;

            template<typename Other>
            struct rebind
            {
                typedef object_generator<Other, Generators> type;
            };

            explicit object_generator(Generators const &gens)
              : gens_(gens)
              , percent_()
            {}

            object_generator(Generators const &gens, int percent)
              : gens_(gens)
              , percent_(percent)
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

            Generators const &gens() const
            {
                return gens_;
            }

            int percent() const
            {
                BOOST_ASSERT(this->has_percent());
                return percent_.get();
            }

            void set_percent(int percent)
            {
                percent_ = percent;
            }

            bool has_percent() const
            {
                return !!percent_;
            }

            object_generator operator%(int percent) const
            {
                return object_generator(gens_, percent);
            }

        private:
            boost::optional<int> percent_;
            Generators gens_;
        };

        template<typename Generators>
        struct constructor
        {
            template<typename Other>
            struct rebind
            {
                typedef object_generator<Other, Generators> type;
            };

            explicit constructor(Generators const &gens)
              : gens_(gens)
              , percent_()
            {}

            constructor(Generators const &gens, int percent)
              : gens_(gens)
              , percent_(percent)
            {}

            Generators const &gens() const
            {
                return gens_;
            }

            int percent() const
            {
                BOOST_ASSERT(this->has_percent());
                return percent_.get();
            }

            void set_percent(int percent)
            {
                percent_ = percent;
            }

            bool has_percent() const
            {
                return !!percent_;
            }

            constructor operator%(int percent) const
            {
                return constructor(gens_, percent);
            }

        private:
            boost::optional<int> percent_;
            Generators gens_;
        };

        template<typename Gens>
        struct constructors
        {
            explicit constructors(Gens const &gens)
              : gens_(gens)
            {}

            template<typename Gen>
            struct result_of_push_back
            {
                typedef constructors<
                    typename fusion::result_of::as_vector<
                        typename fusion::result_of::push_back<Gens, Gen>::type
                    >::type
                > type;
            };

            template<typename Gen>
            typename result_of_push_back<Gen>::type
            push_back(Gen const &gen) const
            {
                typedef typename result_of_push_back<Gen>::type result_type;
                return result_type(fusion::as_vector(fusion::push_back(gens_, gen)));
            }

            Gens const &gens() const
            {
                return gens_;
            }

        private:
            Gens gens_;
        };

        template<typename G0, typename G1>
        constructors<
            typename fusion::result_of::make_vector<
                constructor<G0>
              , constructor<G1>
            >::type
        >
        operator|(constructor<G0> const &g0, constructor<G1> const &g1)
        {
            typedef
                constructors<
                    typename fusion::result_of::make_vector<
                        constructor<G0>
                      , constructor<G1>
                    >::type
                >
            result_type;
            return result_type(fusion::make_vector(g0, g1));
        }

        template<typename Gens, typename G>
        typename constructors<Gens>::template result_of_push_back<constructor<G> >::type
        operator|(constructors<Gens> const &g0, constructor<G> const &g1)
        {
            return g0.push_back(g1);
        }

        template<typename Value, typename Rng>
        struct alt_generator_predicate
        {
            typedef boost::optional<Value> result_type;

            alt_generator_predicate(Rng &rng, int value, int &so_far)
              : rng_(rng)
              , value_(value)
              , so_far_(so_far)
            {}

            template<typename T, typename Gens>
            boost::optional<Value> operator()(boost::optional<Value> const &state, object_generator<T, Gens> &gen) const
            {
                so_far_ += gen.percent();
                if(!state && value_ < so_far_)
                    return gen(rng_);
                return state;
            }
        private:
            Rng &rng_;
            int value_;
            int &so_far_;
        };

        template<typename Gens>
        struct alternate_generator
        {
            explicit alternate_generator(Gens const &gens)
              : percent_dist_(0,99)
              , gens_(gens)
            {}

            typedef
                typename fusion::result_of::value_at_c<Gens, 0>::type::result_type
            result_type;

            template<typename Rng>
            result_type operator()(Rng &rng)
            {
                // Generate a random number between 0 and 99 inclusive
                int percent = percent_dist_(rng);
                int so_far = 0;
                alt_generator_predicate<result_type, Rng> fun(rng, percent, so_far);
                auto result = fusion::fold(gens_, boost::none, fun);
                BOOST_ASSERT(!!result);
                return result.get();
            }

        private:
            boost::random::uniform_int_distribution<int> percent_dist_;
            Gens gens_;
        };

        template<typename Value>
        struct constant_generator
        {
            typedef Value result_type;

            explicit constant_generator(Value const &v)
              : value_(v)
            {}

            template<typename Rng>
            Value operator()(Rng&) const
            {
                return value_;
            }

        private:
            Value value_;
        };

        template<typename Other>
        struct rebind_constructor
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Ctor>
            struct result<This(Ctor)>
            {
                typedef typename Ctor::template rebind<Other>::type type;
            };

            template<typename This, typename Ctor>
            struct result<This(Ctor &)>
              : result<This(Ctor)>
            {};

            template<typename Gens>
            object_generator<Other, Gens> operator()(constructor<Gens> const &ctor) const
            {
                if(ctor.has_percent())
                    return object_generator<Other, Gens>(ctor.gens(), ctor.percent());
                return object_generator<Other, Gens>(ctor.gens());
            }
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
    detail::object_generator<                                                                       \
        T                                                                                           \
      , typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type                   \
    >                                                                                               \
    class_gen(BOOST_PP_ENUM_BINARY_PARAMS(N, const G, &g))                                          \
    {                                                                                               \
        return detail::object_generator<                                                            \
            T                                                                                       \
          , typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type               \
        >(fusion::make_vector(BOOST_PP_ENUM_PARAMS(N, g)));                                         \
    }                                                                                               \
    template<BOOST_PP_ENUM_PARAMS(N, typename G)>                                                   \
    detail::constructor<                                                                            \
        typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type                   \
    >                                                                                               \
    ctor(BOOST_PP_ENUM_BINARY_PARAMS(N, const G, &g))                                               \
    {                                                                                               \
        return detail::constructor<                                                                 \
            typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type               \
        >(fusion::make_vector(BOOST_PP_ENUM_PARAMS(N, g)));                                         \
    }                                                                                               \
    /**/
#define BOOST_PP_LOCAL_LIMITS (1, QCHK_MAX_ARITY)
#include BOOST_PP_LOCAL_ITERATE()

    template<typename T, typename Gens>
    detail::alternate_generator<
        typename fusion::result_of::as_vector<
            typename fusion::result_of::transform<Gens, detail::rebind_constructor<T> >::type
        >::type
    >
    class_gen(detail::constructors<Gens> const &ctors)
    {
        return detail::alternate_generator<
            typename fusion::result_of::as_vector<
                typename fusion::result_of::transform<Gens, detail::rebind_constructor<T> >::type
            >::type
        >(fusion::as_vector(fusion::transform(ctors.gens(), detail::rebind_constructor<T>())));
    }

}

QCHK_BOOST_NAMESPACE_END

#endif
