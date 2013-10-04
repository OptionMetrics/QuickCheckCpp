////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file config.hpp
/// \brief Definition of \c quick_check::config\<\> and \c quick_check::make_config() for defining
///        test configurations.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_CONFIG_HPP_INCLUDED
#define QCHK_CONFIG_HPP_INCLUDED

#include <utility>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/map/convert.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/algorithm/query/find_if.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/algorithm/transformation/push_front.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/algorithm/transformation/filter_if.hpp>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator.hpp>
#include <boost/quick_check/detail/functional.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/is_placeholder.hpp>
#include <boost/random/mersenne_twister.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct rng_ {};
        struct test_count_ {};
        struct max_test_count_ {};
        struct sized_ {};

        struct PhxPlaceholder
          : proto::and_<
                proto::terminal<proto::_>
              , proto::if_<boost::is_placeholder<proto::_value>()>
            >
        {};

        struct DoInsert
          : proto::transform<DoInsert>
        {
            template<typename Expr, typename State, typename Data>
            struct impl
              : proto::transform_impl<Expr, State, Data>
            {
                typedef
                    typename std::remove_const<
                        typename std::remove_reference<
                            typename proto::result_of::value<
                                typename proto::result_of::left<Expr>::type
                            >::type
                        >::type
                    >::type
                key_type;

                typedef
                    std::pair<
                        typename fusion::result_of::as_map<
                            typename fusion::result_of::push_back<
                                typename State::first_type
                              , fusion::pair<key_type, typename State::second_type>
                            >::type
                        >::type
                      , key_type
                    >
                result_type;

                result_type operator()(typename impl::expr_param e,
                                       typename impl::state_param s,
                                       typename impl::data_param) const
                {
                    return std::make_pair(
                        fusion::as_map(
                            fusion::push_back(
                                s.first
                              , fusion::make_pair<key_type>(s.second)
                            )
                        )
                      , proto::value(proto::left(e))
                    );
                }
            };
        };

        struct CollectTerminal
          : proto::call<proto::functional::make_pair(
                proto::make<fusion::map<>()>
              , proto::_value
            )>
        {};

        struct RngCollection
          : proto::or_<
                proto::when<
                    proto::assign<PhxPlaceholder, proto::terminal<proto::_> >
                  , DoInsert(proto::_, CollectTerminal(proto::_right))
                >
              , proto::when<
                    proto::assign<PhxPlaceholder, RngCollection>
                  , DoInsert(proto::_, RngCollection(proto::_right))
                >
            >
        {};

        struct fusion_join
          : proto::callable, detail::binary<>
        {
            template<typename A, typename B>
            typename fusion::result_of::as_vector<
                fusion::joint_view<A const, B const>
            >::type
            operator()(A const &a, B const &b) const
            {
                return fusion::as_vector(fusion::join(a, b));
            }
        };

        template<typename Map, typename Placeholder>
        struct safe_value_at_key
          : mpl::eval_if<
                fusion::result_of::has_key<Map, Placeholder>
              , fusion::result_of::value_at_key<Map, Placeholder>
              , mpl::identity<fusion::void_>
            >
        {};

        template<typename Map
               , typename Rng
               , typename Placeholder
               , typename ValueAtKey = typename safe_value_at_key<Map, Placeholder>::type>
        struct my_value_at_key
        {
            typedef typename std::result_of<ValueAtKey(Rng &)>::type type;
            static type call(Map &map, Rng &rng)
            {
                return fusion::at_key<Placeholder>(map)(rng);
            }
        };

        template<typename Map, typename Rng, typename Placeholder, int I>
        struct my_value_at_key<Map, Rng, Placeholder, phoenix::argument<I> >
        {
            typedef typename my_value_at_key<Map, Rng, phoenix::argument<I> >::type type;
            static type call(Map &map, Rng &rng)
            {
                return my_value_at_key<Map, Rng, phoenix::argument<I> >::call(map, rng);
            }
        };

        template<typename Map, typename Rng, typename Placeholder>
        struct my_value_at_key<Map, Rng, Placeholder, fusion::void_>
        {
            typedef fusion::void_ type;
            static type call(Map &, Rng &)
            {
                return fusion::void_();
            }
        };

        template<typename T>
        struct as_size_t
          : mpl::size_t<T::value>
        {};

        template<std::size_t I, typename State = fusion::vector<> >
        struct make_indices
          : make_indices<
                I-1
              , typename fusion::result_of::push_front<State, phoenix::argument<(int)I> >::type
            >
        {};

        template<typename State>
        struct make_indices<0, State>
          : fusion::result_of::as_vector<State>
        {};

        template<typename Map, typename Rng>
        struct index_fun
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Placeholder>
            struct result<This(Placeholder)>
            {
                typedef
                    typename detail::my_value_at_key<
                        Map
                      , Rng
                      , typename boost::remove_const<
                            typename boost::remove_reference<Placeholder>::type
                        >::type
                    >::type
                type;
            };

            index_fun(Map &map, Rng &rng)
              : map_(map)
              , rng_(rng)
            {}

            template<typename Placeholder>
            typename detail::my_value_at_key<Map, Rng, Placeholder>::type
            operator()(Placeholder) const
            {
                return detail::my_value_at_key<Map, Rng, Placeholder>::call(this->map_, this->rng_);
            }

        private:
            index_fun &operator=(index_fun const &);
            Map &map_;
            Rng &rng_;
        };

        template<typename Key, typename Callable = proto::callable>
        struct KeyValue
          : proto::when<
                proto::assign<
                    proto::terminal<Key>
                  , proto::terminal<proto::_>
                >
              , proto::_value(proto::_right)
            >
        {};

        typedef KeyValue<rng_> RngValue;
        typedef KeyValue<test_count_> TestCountValue;
        typedef KeyValue<max_test_count_> MaxTestCountValue;
        typedef KeyValue<sized_> SizedValue;

        template<typename Expr>
        struct is_rng_collection
          : proto::matches<Expr, detail::RngCollection>
        {};

        template<typename Args>
        typename fusion::result_of::as_map<
            typename boost::remove_reference<
                typename fusion::result_of::accumulate<
                    typename fusion::result_of::transform<
                        typename fusion::result_of::transform<
                            typename fusion::result_of::filter_if<
                                Args
                              , mpl::quote1<is_rng_collection>
                            >::type
                          , detail::RngCollection
                        >::type
                      , proto::functional::first
                    >::type
                  , fusion::nil_
                  , detail::fusion_join
                >::type
            >::type
        >::type
        make_config_map(Args args)
        {
            auto a = fusion::as_vector(fusion::filter_if<mpl::quote1<is_rng_collection> >(args));
            auto b = fusion::as_vector(fusion::transform(a, detail::RngCollection()));
            auto c = fusion::as_vector(fusion::transform(b, proto::functional::first()));
            auto d = fusion::as_vector(fusion::accumulate(c, fusion::nil_(), detail::fusion_join()));
            return fusion::as_map(d);
        }
    }

    /// \brief A structure that holds all the test configuration information
    /// for use with the \c quick_check::qcheck() algorithm.
    ///
    /// \tparam Map \em unspecified
    /// \tparam Rng \em unspecified
    ///
    /// You will typically not create objects of type \c config\<\> directly; rather,
    /// you will use the \c quick_check::make_config() helper to do it for you.
    template<typename Map, typename Rng>
    struct config
    {
    private:
        typedef
            typename mpl::accumulate<
                Map
              , mpl::size_t<0>
              , mpl::max<mpl::_1, detail::as_size_t<fusion::result_of::first<mpl::_2> > >
            >::type
        arg_count;

        typedef
            typename detail::make_indices<arg_count::value>::type
        indices_type;

    public:
        /// The type returned by quick_check::config::operator().
        /// \c result_type is Fusion sequence containing arguments
        /// that will be used to evaluate a property.
        typedef
            typename fusion::result_of::as_vector<
                typename fusion::result_of::transform<
                    indices_type
                  , detail::index_fun<Map, Rng>
                >::type
            >::type
        result_type;

        /// Construct a \c config object
        ///
        /// \param map \em unspecified
        /// \param rng \em unspecified
        /// \param test_count The maximum number of tests to run.
        /// \param max_test_count The maximum number of inputs to generate.
        /// \param sized Used to control the size of any generated sequences.
        config(
            Map const &map
          , Rng const &rng
          , std::size_t test_count
          , std::size_t max_test_count
          , std::size_t sized
        )
          : map_(map)
          , rng_(rng)
          , test_count_(test_count)
          , max_test_count_(std::max(test_count, max_test_count))
          , sized_(sized)
        {
            this->resized(sized);
        }

        /// Returns the value of \c test_count passed to the constructor
        std::size_t test_count() const
        {
            return this->test_count_;
        }

        /// Returns the max of \c test_count and \c max_test_count passed to the constructor
        std::size_t max_test_count() const
        {
            return this->max_test_count_;
        }

        /// Returns the value of \c sized passed to the constructor
        std::size_t sized() const
        {
            return this->sized_;
        }

        /// Sets the sized property of any internally stored generators
        void resized(std::size_t sized)
        {
            this->sized_ = sized;
            typedef proto::functional::second F;
            fusion::for_each(
                fusion::transform_view<Map, F>(this->map_, F())
              , detail::set_size(this->sized_)
            );
        }

        /// Generates a new, random set of input parameters for use
        /// when evaluating a property.
        result_type operator()()
        {
            return fusion::as_vector(
                fusion::transform(
                    indices_type()
                  , detail::index_fun<Map, Rng>(this->map_, this->rng_)
                )
            );
        }

    private:
        Map map_; // A map from phx placeholders to generators or other phx placeholders
        Rng rng_;
        std::size_t test_count_;
        std::size_t max_test_count_;
        std::size_t sized_;
    };

    /// A placeholder for use with \c quick_check::make_config() for specifying
    /// a custom random number generator to use.
    ///
    /// If \c _rng is not specified, it defaults to a default-constructed object
    /// of type \c boost::random::mt11213b.
    ///
    /// \b Example:
    ///
    /*! \code
        // Generate alphabetic characters, using rng as
        // a source of randomness.
        boost::random::mt19937 rng;
        auto conf = make_config(_1 = alpha(), _rng = rng);
        \endcode
    */
    proto::terminal<detail::rng_>::type const _rng = {};

    /// A placeholder for use with \c quick_check::make_config() for specifying
    /// the maximum number of tests to run.
    ///
    /// If \c _test_count is not specified, it defaults to 100u.
    ///
    /// \b Example:
    ///
    /*! \code
        // Make a config that will cause quick_book::qcheck() to attempt
        // to run 1,000 test cases.
        auto conf = make_config(_1 = alpha(), _test_count = 1000u);
        \endcode
    */
    proto::terminal<detail::test_count_>::type const _test_count = {};

    /// A placeholder for use with \c quick_check::make_config() for specifying
    /// the maximum number of inputs to generate.
    ///
    /// If \c _max_test_count is not specified, it defaults to \c _test_count or
    /// 1000u, whichever is larger.
    ///
    /// \remark The number of inputs generated may differ from the number of tests
    /// actually run because of the use of condition property guards. If a
    /// set of inputs does not satisfy a property's
    /// \RefSect{users_guide.properties.conditional, condition predicate}, then
    /// those inputs are discarded. The \c _max_test_count configuration parameter
    /// exits to bound the number of inputs generated, in case the condition
    /// predicate discards too many inputs.
    ///
    /// \b Example:
    ///
    /*! \code
        // Make a config that generates no more than 1,000 alphabetic characters.
        auto conf = make_config(_1 = alpha(), _max_test_count = 1000u);
        \endcode
    */
    proto::terminal<detail::max_test_count_>::type const _max_test_count = {};

    /// A placeholder for use with \c quick_check::make_config() for specifying
    /// the maximum size of generated sequences.
    ///
    /// The \c _sized parameter places an upper bound on the size of generated
    /// sequences. The actual size of generated sequences is random and uniformly
    /// distributed between 0 and \c _sized.
    ///
    /// If \c _sized is not specified, it defaults to 50.
    ///
    /// \b Example:
    ///
    /*! \code
        // Make a config that generates strings with a maximum size of 10.
        auto conf = make_config(_1 = string(), _sized = 10u);
        \endcode
    */
    proto::terminal<detail::sized_>::type const _sized = {};

    namespace detail
    {
        template<typename Grammar, typename Args>
        auto fetch_arg(Args const &args)
        QCHK_RETURN(
            Grammar()(*fusion::find_if<proto::matches<mpl::_, Grammar> >(args))
        )

        template<typename Map, typename Rng>
        config<Map, Rng> make_config_impl(
            Map const &map
          , Rng const &rng
          , std::size_t test_count
          , std::size_t max_test_count
          , std::size_t sized
        )
        {
            return config<Map, Rng>(map, rng, test_count, max_test_count, sized);
        }

        template<typename Args, typename ArgsWithDefaults>
        auto make_config_(Args const &args, ArgsWithDefaults const &args_with_defaults)
        QCHK_RETURN(
            detail::make_config_impl(
                detail::make_config_map(args)
              , detail::fetch_arg<RngValue>(args_with_defaults)
              , detail::fetch_arg<TestCountValue>(args_with_defaults)
              , detail::fetch_arg<MaxTestCountValue>(args_with_defaults)
              , detail::fetch_arg<SizedValue>(args_with_defaults)
            )
        )
    }

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)

    namespace detail
    {
        template<typename...As>
        struct result_of_make_config
        {
            typedef decltype(
                detail::make_config_(
                    fusion::make_vector(boost::declval<As const &>()...)
                  , fusion::make_vector(
                        boost::declval<As const &>()...
                      , _rng = boost::random::mt11213b()
                      , _test_count = 100u
                      , _max_test_count = 1000u
                      , _sized = 50u
                    )
                )
            ) type;
        };
    }

    /// Create an instance of \c quick_check::config\<\> with the specified
    /// argument placeholder/generator bindings, and any additional specified
    /// configuration parameters.
    ///
    /// \param as Must contain argument placeholder/generator bindings of the form
    /// <tt>_1 = gen</tt> for every argument of the property to be tested. May also
    /// contain placeholder/value bindings like <tt>_sized = 42u</tt> for any
    /// addtional configuration parameters to be specified.
    ///
    /// \return An unspecified instance of \c quick_check::config\<\>.
    ///
    /// \em Example:
    ///
    /*! \code
        // A source for random numbers
        boost::random::mt11213b rng;

        // some generators
        uniform<int> die(1,6);
        normal<double> one(0.0, 1.0);

        // a QuickCheckCpp configuration. Placeholders _1
        // and _2 receive values generated from die and one:
        auto config =
            make_config(_1 = die,
                        _2 = one,
                        _rng = rng,               // Specify a custom random number generator
                        _test_count = 10000,      // The maximum number of tests to execute
                        _max_test_count = 15000); // The maximum number of inputs to generate
    \endcode
    */
    /// Do not use the same generator with two different argument placeholders, like
    /// <tt>make_config(_1 = die, _2 = die)</tt>. Both generators will be copied and
    /// they will have the same internal state while generating random numbers,
    /// resulting in poor randomness. Instead, you can reuse the same generator like
    /// this:
    ///
    /*! \code
        // OK, reuse the same generator for 2 placeholders.
        uniform<int> die(1,6);
        auto conf = make_config(_1 = _2 = die);
        \endcode
    */
    ///
    /// \sa \c quick_check::_rng
    /// \sa \c quick_check::_test_count
    /// \sa \c quick_check::_max_test_count
    /// \sa \c quick_check::_sized
    template<typename ...As>
    typename detail::result_of_make_config<As...>::type
    make_config(As const &... as)
    {
        return detail::make_config_(
            fusion::make_vector(as...)
          , fusion::make_vector(
                as...
              , _rng = boost::random::mt11213b()
              , _test_count = 100u
              , _max_test_count = 1000u
              , _sized = 50u
            )
        );
    }

#else
    inline auto make_config()
    QCHK_RETURN(
        detail::make_config_impl(
            fusion::vector0<>()
          , boost::random::mt11213b()
          , 100u
          , 1000u
          , 50u
        )
    )

#define BOOST_PROTO_LOCAL_MACRO(N, typename_A, A_const_ref, A_const_ref_a, a)           \
                                                                                        \
    template<typename_A(N)>                                                             \
    auto make_config(A_const_ref_a(N))                                                  \
    QCHK_RETURN(                                                                        \
        detail::make_config_(                                                           \
            fusion::make_vector(a(N))                                                   \
          , fusion::make_vector(                                                        \
                a(N)                                                                    \
                BOOST_PP_COMMA_IF(N)                                                    \
                _rng = boost::random::mt11213b()                                        \
              , _test_count = 100u                                                      \
              , _max_test_count = 1000u                                                 \
              , _sized = 50u                                                            \
            )                                                                           \
        )                                                                               \
    )                                                                                   \
    /**/

#define BOOST_PROTO_LOCAL_a BOOST_PROTO_a
#define BOOST_PROTO_LOCAL_LIMITS (1, BOOST_PP_DEC(QCHK_MAX_ARITY))
#include BOOST_PROTO_LOCAL_ITERATE()
#endif

}

QCHK_BOOST_NAMESPACE_END

#endif
