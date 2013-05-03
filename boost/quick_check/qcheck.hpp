////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QCHECK_HPP_INCLUDED
#define QCHK_QCHECK_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/quick_check/detail/array.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct unpack_array
        {
            template<typename Sig>
            struct result
            {};

            template<typename This, typename T>
            struct result<This(T)>
            {
                typedef T type;
            };

            template<typename This, typename T, std::size_t N>
            struct result<This(detail::array<T[N]>)>
            {
                typedef boost::array<T, N> type;
            };

            template<typename This, typename T>
            struct result<This(T &)>
              : result<This(T)>
            {};

            template<typename This, typename T>
            struct result<This(T const &)>
              : result<This(T)>
            {};

            template<typename T>
            T operator()(T const &t) const
            {
                return t;
            }

            template<typename T, std::size_t N>
            boost::array<T, N> operator()(detail::array<T[N]> const &rg) const
            {
                return rg.elems;
            }
        };

        template<typename Seq, std::size_t I>
        struct safe_at_c
          : mpl::eval_if_c<
                I >= fusion::result_of::size<Seq>::value
              , mpl::identity<void>
              , fusion::result_of::value_at_c<Seq, I>
            >
        {};

        #define TYPENAME(Z,N,D) typename
        #define AT_C(Z,N,D) typename safe_at_c<D, N>::type
        template<typename Seq, template<BOOST_PP_ENUM(QCHK_MAX_ARITY, TYPENAME, ~)> class T>
        struct fanout_params
        {
            typedef T<BOOST_PP_ENUM(QCHK_MAX_ARITY, AT_C, Seq)> type;
        };
        #undef AT_C
        #undef TYPENAME
    }

    template<typename Property, typename Config>
    typename detail::fanout_params<
        typename fusion::result_of::as_vector<
            typename fusion::result_of::transform<
                typename Config::args_type
              , detail::unpack_array
            >::type
        >::type
      , qcheck_results
    >::type

    //qcheck_results<
    //    typename fusion::result_of::as_vector<
    //        typename fusion::result_of::transform<
    //            typename Config::args_type
    //          , detail::unpack_array
    //        >::type
    //    >::type
    //>
    qcheck(Property const &prop, Config &config)
    {
        typedef
            typename detail::fanout_params<
                typename fusion::result_of::as_vector<
                    typename fusion::result_of::transform<
                        typename Config::args_type
                      , detail::unpack_array
                    >::type
                >::type
              , qcheck_results
            >::type
        results_type;

        //typedef
        //    typename fusion::result_of::as_vector<
        //        typename fusion::result_of::transform<
        //            typename Config::args_type
        //          , detail::unpack_array
        //        >::type
        //    >::type
        //args_type;

        results_type results;

        for(std::size_t n = 0; n < config.test_count(); ++n)
        {
            auto args = config.gen();
            // The static_cast here is so that operator! doesn't get invoked
            // for detail::array, which returns a new array that is the logical
            // negation of each element.
            if(!static_cast<bool>(fusion::invoke_function_object(prop, args)))
            {
                results.failures.push_back(
                    fusion::as_vector(fusion::transform(args, detail::unpack_array()))
                );
            }
        }

        return results;
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
