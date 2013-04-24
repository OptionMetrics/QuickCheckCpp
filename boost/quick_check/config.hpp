////////////////////////////////////////////////////////////////////////////////////////////////////
// \file config.hpp
// \brief Definition of test configuration objects
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_CONFIG_HPP_INCLUDED
#define QCHK_CONFIG_HPP_INCLUDED

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/map/convert.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/view/joint_view.hpp>
#include <boost/fusion/algorithm/transformation/push_front.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/is_placeholder.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Map>
        config<Map> make_config(Map const &map)
        {
            return config<Map>(map);
        }

        struct PhxPlaceholder
          : proto::and_<
                proto::terminal<proto::_>
              , proto::if_<boost::is_placeholder<proto::_value>()>
            >
        {};

        struct Tmp
          : proto::call<proto::functional::make_pair(
                proto::call<proto::functional::push_back(
                    proto::call<proto::functional::first(proto::_state)>
                  , proto::make<fusion::pair<
                        proto::call<proto::_value(proto::_left)>
                      , proto::call<proto::functional::second(proto::_state)>
                    >(proto::functional::second(proto::_state))>
                )>
              , proto::call<proto::_value(proto::_left)>
            )>
        {};

        struct RngCollection
          : proto::or_<
                proto::when<
                    proto::assign<PhxPlaceholder, proto::terminal<proto::_> >
                  , proto::call<proto::functional::make_pair(
                        proto::call<proto::functional::push_back(
                            proto::make<fusion::map<>()>
                          , proto::make<fusion::pair<
                                proto::_value(proto::_left)
                              , proto::_value(proto::_right)
                            >(proto::_value(proto::_right))>
                        )>
                      , proto::_value(proto::_left)
                    )>
                >
              , proto::when<
                    proto::assign<PhxPlaceholder, RngCollection>
                  , proto::call<Tmp(proto::_, RngCollection(proto::_right))>
                >
            >
        {};

        struct fusion_join
        {
            template<typename Sig>
            struct result;

            template<typename This, typename A, typename B>
            struct result<This(A,B)>
            {
                typedef
                    fusion::joint_view<
                        typename boost::add_const<typename boost::remove_reference<A>::type>::type
                      , typename boost::add_const<typename boost::remove_reference<B>::type>::type
                    >
                type;
            };

            template<typename A, typename B>
            fusion::joint_view<A const, B const>
            operator()(A const &a, B const &b) const
            {
                return fusion::joint_view<A const, B const>(a, b);
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
               , typename Placeholder
               , typename ValueAtKey = typename safe_value_at_key<Map, Placeholder>::type>
        struct my_value_at_key
        {
            typedef typename boost::result_of<ValueAtKey()>::type type;
            static type call(Map &map)
            {
                return fusion::at_key<Placeholder>(map)();
            }
        };

        template<typename Map, typename Placeholder, int I>
        struct my_value_at_key<Map, Placeholder, phoenix::argument<I> >
        {
            typedef typename my_value_at_key<Map, phoenix::argument<I> >::type type;
            static type call(Map &map)
            {
                return my_value_at_key<Map, phoenix::argument<I> >::call(map);
            }
        };

        template<typename Map, typename Placeholder>
        struct my_value_at_key<Map, Placeholder, fusion::void_>
        {
            typedef fusion::void_ type;
            static type call(Map &map)
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

        template<typename Map>
        struct index_fun
        {
            template<typename Sig>
            struct result;
            
            template<typename This, typename Placeholder>
            struct result<This(Placeholder)>
              : detail::my_value_at_key<
                    Map
                  , typename boost::remove_const<
                        typename boost::remove_reference<Placeholder>::type
                    >::type
                >
            {};

            explicit index_fun(Map & map)
              : map_(map)
            {}

            template<typename Placeholder>
            typename detail::my_value_at_key<Map, Placeholder>::type
            operator()(Placeholder) const
            {
                return detail::my_value_at_key<Map, Placeholder>::call(this->map_);
            }

        private:
            Map &map_;
        };
    }

    template<typename Map>
    struct config
    {
        typedef
            typename mpl::accumulate<
                typename mpl::transform<
                    Map
                  , detail::as_size_t<fusion::result_of::first<mpl::_> >
                >::type
              , mpl::size_t<0>
              , mpl::max<mpl::_1, mpl::_2>
            >::type
        arg_count;

        typedef
            typename detail::make_indices<arg_count::value>::type
        indices_type;

        typedef
            typename fusion::result_of::as_vector<
                typename fusion::result_of::transform<
                    indices_type
                  , detail::index_fun<Map>
                >::type
            >::type
        args_type;

        explicit config(Map const &map)
          : map_(map)
        {}

        std::size_t test_count() const
        {
            return 100;
        }

        args_type gen()
        {
            return fusion::as_vector(
                fusion::transform(indices_type(), detail::index_fun<Map>(this->map_))
            );
        }

        template<typename Placeholder>
        typename detail::my_value_at_key<
            Map
          , typename proto::result_of::value<Placeholder>::type
        >::type
        next(Placeholder const &placeholder)
        {
            return detail::my_value_at_key<
                Map
              , typename proto::result_of::value<Placeholder>::type
            >::call(this->map_);
        }

    private:
        Map map_;
    };

#define BOOST_PROTO_LOCAL_MACRO(N, typename_A, A_const_ref, A_const_ref_a, a)   \
    template<typename_A(N)>                                                     \
    auto make_config(A_const_ref_a(N))                                          \
    QCHK_RETURN(                                                                \
        detail::make_config(                                                    \
            fusion::as_map(                                                     \
                fusion::accumulate(                                             \
                    fusion::transform(                                          \
                        fusion::transform(                                      \
                            fusion::make_vector(a(N))                           \
                          , detail::RngCollection()                             \
                        )                                                       \
                      , proto::functional::first()                              \
                    )                                                           \
                  , fusion::nil_()                                              \
                  , detail::fusion_join()                                       \
                )                                                               \
            )                                                                   \
        )                                                                       \
    )                                                                           \
    /**/
#define BOOST_PROTO_LOCAL_a BOOST_PROTO_a
#define BOOST_PROTO_LOCAL_LIMITS (1, BOOST_PP_DEC(QCHK_MAX_ARITY))
#include BOOST_PROTO_LOCAL_ITERATE()

}

QCHK_BOOST_NAMESPACE_END

#endif
