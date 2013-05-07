///////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck.hpp
// \brief Definition of qcheck algorithm
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_PROPERTY_HPP_INCLUDED
#define QCHK_PROPERTY_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/quick_check/detail/grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename T>
        struct wrap_array
        {
            typedef T type;
        };

        template<typename T, std::size_t N>
        struct wrap_array<T[N]>
        {
            typedef detail::array<T[N]> type;
        };

        template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
        struct property_traits
        {
            typedef bool call_signature(
                BOOST_PP_ENUM_BINARY_PARAMS(
                    QCHK_MAX_ARITY
                  , typename wrap_array<A, >::type BOOST_PP_INTERCEPT
                )
            );
            typedef typename
                fusion::result_of::make_vector<
                    BOOST_PP_ENUM_BINARY_PARAMS(
                        QCHK_MAX_ARITY
                      , typename wrap_array<A, >::type BOOST_PP_INTERCEPT
                    )
                >::type
            args_type;
        };

    #define BOOST_PP_LOCAL_MACRO(N)                                                                 \
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>                                               \
        struct property_traits<BOOST_PP_ENUM_PARAMS(N, A)>                                          \
        {                                                                                           \
            typedef bool call_signature(                                                            \
                BOOST_PP_ENUM_BINARY_PARAMS(N, typename wrap_array<A, >::type BOOST_PP_INTERCEPT)   \
            );                                                                                      \
            typedef typename                                                                        \
                fusion::result_of::make_vector<                                                     \
                    BOOST_PP_ENUM_BINARY_PARAMS(N, typename wrap_array<A, >::type BOOST_PP_INTERCEPT) \
                >::type                                                                             \
            args_type;                                                                              \
        };                                                                                          \
        /**/

    #define BOOST_PP_LOCAL_LIMITS (1, BOOST_PP_DEC(QCHK_MAX_ARITY))
    #include BOOST_PP_LOCAL_ITERATE()
    }

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct property
      : private boost::function<
            typename detail::property_traits<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::call_signature
        >
    {
    private:
        typedef typename
            detail::property_traits<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::call_signature
        sig_type;

        typedef typename
            detail::property_traits<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::args_type
        args_type;

        boost::function<std::string(args_type)> classifier_;
    public:
        template<typename Actor>
        property(phoenix::actor<Actor> const &actor)
          : boost::function<sig_type>(detail::GetProperty()(actor))
          , classifier_(detail::GetClassifier()(actor))
        {}

        typedef bool result_type; // for TR1
        using boost::function<sig_type>::operator();

        typedef boost::function<std::string(args_type)> classifier_type;
        classifier_type const &classifier() const
        {
            return this->classifier_;
        }
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
