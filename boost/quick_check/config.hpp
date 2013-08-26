////////////////////////////////////////////////////////////////////////////////////////////////////
// \file config.hpp
// \brief Definition of test configuration objects
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
        typedef
            typename fusion::result_of::as_vector<
                typename fusion::result_of::transform<
                    indices_type
                  , detail::index_fun<Map, Rng>
                >::type
            >::type
        result_type;

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

        std::size_t test_count() const
        {
            return this->test_count_;
        }

        std::size_t max_test_count() const
        {
            return this->max_test_count_;
        }

        std::size_t sized() const
        {
            return this->sized_;
        }

        void resized(std::size_t sized)
        {
            this->sized_ = sized;
            typedef proto::functional::second F;
            fusion::for_each(
                fusion::transform_view<Map, F>(this->map_, F())
              , detail::set_size(this->sized_)
            );
        }

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

    proto::terminal<detail::rng_>::type const _rng = {};
    proto::terminal<detail::test_count_>::type const _test_count = {};
    proto::terminal<detail::max_test_count_>::type const _max_test_count = {};
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
                    fusion::make_vector(std::declval<As const &>()...)
                  , fusion::make_vector(
                        std::declval<As const &>()...
                      , _rng = boost::random::mt11213b()
                      , _test_count = 100u
                      , _max_test_count = 1000u
                      , _sized = 50u
                    )
                )
            ) type;
        };
    }

    /// @brief Do the thing
    /// @throw nothing
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
