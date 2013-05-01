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
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/enum_params_with_a_default.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/qcheck_results.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
        struct property_impl
        {
            typedef bool type(BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A));
        };

    #define BOOST_PP_LOCAL_MACRO(N)                                                                 \
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>                                               \
        struct property_impl<BOOST_PP_ENUM_PARAMS(N, A)>                                            \
        {                                                                                           \
            typedef bool type(BOOST_PP_ENUM_PARAMS(N, A));                         \
        };                                                                                          \
        /**/

    #define BOOST_PP_LOCAL_LIMITS (1, BOOST_PP_DEC(QCHK_MAX_ARITY))
    #include BOOST_PP_LOCAL_ITERATE()
    }

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct property
      : private boost::function<
            typename detail::property_impl<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::type
        >
    {
    private:
        typedef typename
            detail::property_impl<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::type
        sig_type;

    public:
        template<typename Actor>
        property(phoenix::actor<Actor> const &actor)
          : boost::function<sig_type>(actor)
        {}

        typedef bool result_type; // for TR1
        using boost::function<sig_type>::operator();
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
