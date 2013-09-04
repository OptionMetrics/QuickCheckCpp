///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file class.hpp
/// \brief Definition of \c quick_check::class_\<\>, for generating objects of user-defined types.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_CLASS_HPP_INCLUDED
#define QCHK_GENERATOR_CLASS_HPP_INCLUDED

#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/algorithm/transformation/push_back.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>
#include <boost/quick_check/detail/array.hpp>

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

        /// INTERNAL ONLY
#define BOOST_PP_LOCAL_MACRO(N)                                                                     \
            template<BOOST_PP_ENUM_PARAMS(N, typename T)>                                           \
            Type operator()(BOOST_PP_ENUM_BINARY_PARAMS(N, const T, &t)) const                      \
            {                                                                                       \
                return Type(BOOST_PP_ENUM_PARAMS(N, t));                                            \
            }                                                                                       \
            /**/

        /// INTERNAL ONLY
#define BOOST_PP_LOCAL_LIMITS (1, QCHK_MAX_ARITY)
#include BOOST_PP_LOCAL_ITERATE()
        };

        // For use when running generators stored in a Fusion sequence.
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

            friend void set_size_adl(object_generator &thiz, std::size_t size)
            {
                fusion::for_each(thiz.gens_, detail::set_size(size));
            }

        private:
            Generators gens_;
            boost::optional<int> percent_;
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
            Generators gens_;
            boost::optional<int> percent_;
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
            boost::optional<Value> operator()(
                boost::optional<Value> const &state,
                object_generator<T, Gens> const &gen
            ) const
            {
                so_far_ += gen.percent();
                if(!state && value_ < so_far_)
                    return const_cast<object_generator<T, Gens> &>(gen)(rng_);
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
                boost::optional<result_type> state = boost::none;
                alt_generator_predicate<result_type, Rng> fun(rng, percent, so_far);
                state = fusion::fold(gens_, state, fun);
                BOOST_ASSERT(!!state);
                return state.get();
            }

            friend void set_size_adl(alternate_generator &thiz, std::size_t size)
            {
                fusion::for_each(thiz.gens_, detail::set_size(size));
            }

        private:
            boost::random::uniform_int_distribution<int> percent_dist_;
            Gens gens_;
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

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
    /// \brief A helper for \c quick_check::class_() for describing a constructor
    /// with which objects of a class type can be constructed, and with what
    /// frequency.
    ///
    /// \param gs Generators that are used to generate the arguments for this
    ///           constructor, which it is used.
    ///
    /// Some classes have multiple constructors. A generator must know which
    /// constructors(s) to call, and how frequently to call each. That is the
    /// purpose of \c ctor.
    ///
    /// The \c ctor helper is used in conjunction with \c quick_check::class_()
    /// as shown below:
    ///
    /*! \code
        struct S
        {
            S(int);
            S(int, double);
        };

        // ...

        uniform<int> i(1,6);
        normal<double> d(0.,1.);

        // genS is a generator for S objects that calls S::S(int, double)
        // 50% of the time, and S::S(int) 50% of the time.
        auto genS = class_<S>( ctor(i, d) %50
                             | ctor(i)    %50
                             );
        \endcode
    */
    ///
    /// \sa \c quick_check::class_()
    template<typename ...Gs>
    detail::constructor<
        typename fusion::result_of::make_vector<Gs...>::type
    >
    ctor(Gs const &... gs)
    {
        return detail::constructor<
            typename fusion::result_of::make_vector<Gs...>::type
        >(fusion::make_vector(gs...));
    }

    /// \brief For defining a generator for a class type.
    ///
    /// \tparam T  The (class) type of the objects that will be generated.
    /// \tparam Gs Generators that will be used to generate the arguments
    ///            to \c T's constructor.
    ///
    /// Use \c class_() for defining a generator for a class type. With
    /// \c class_(), you can specify a type and a constructor signature,
    /// or multiple signatures with a percentage for each. That constructor
    /// will be called that percent of the time.
    ///
    /// Given a class \c S declared as follows:
    ///
    /*! \code
        struct S
        {
            S(int);
            S(int, double);
        };
        \endcode
    */
    ///   
    /// You can define a generator \c genS that produces \c S objects
    /// by constructing them with random integers as follows:
    ///
    /*! \code
        uniform<int> die(1,6);
        auto genS = class_(die);

        boost::random::mt11213b rng;
        S s = genS(rng); // OK, generate an S object with a random int
        \endcode
    */
    ///
    /// \c class_() is a composite generator. You initialize it with one or
    /// more other generators, which are used to supply the constructor argument(s).
    /// The above generator \c genS uses the generator \c die to generate random
    /// integers between 1 and 6 inclusive. Those integers are used to construct
    /// new \c S objects.
    ///
    /// The \c S class above has more than one constructor. To specify multiple
    /// constructor signatures, and how often to call each, we use the \c ctor
    /// helper. The code below calls each of \c S's constructors 50% of the time:
    ///
    /*! \code
        uniform<int> die(1,6);
        normal<double> one(0.0,1.0);
        auto genS = class_(ctor(die)      %50
                          |ctor(die, one) %50);

        boost::random::mt11213b rng;
        S s = genS(rng); // OK, generate an S object with random args
        \endcode
    */
    ///
    /// The code above will construct an \c S object using one of its two constructors
    /// with random arguments.
    ///
    /// \sa \c quick_check::ctor()
    template<typename T, typename...Gs>
    detail::object_generator<
        T
      , typename fusion::result_of::make_vector<Gs...>::type
    >
    class_(Gs const &... gs)
    {
        return detail::object_generator<
            T
          , typename fusion::result_of::make_vector<Gs...>::type
        >(fusion::make_vector(gs...));
    }
#else
    /// \brief Create a custom generator for a type given several other generators to use
    ///        for generating constructor arguments.
#define BOOST_PP_LOCAL_MACRO(N)                                                                     \
    template<typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, typename G)>                               \
    detail::object_generator<                                                                       \
        T                                                                                           \
      , typename fusion::result_of::make_vector<BOOST_PP_ENUM_PARAMS(N, G)>::type                   \
    >                                                                                               \
    class_(BOOST_PP_ENUM_BINARY_PARAMS(N, const G, &g))                                             \
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
#endif

    /// \overload
    ///
    template<typename T, typename Gens>
    detail::alternate_generator<
        typename fusion::result_of::as_vector<
            typename fusion::result_of::transform<Gens, detail::rebind_constructor<T> >::type
        >::type
    >
    class_(detail::constructors<Gens> const &ctors)
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
