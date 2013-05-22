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
#include <boost/preprocessor/punctuation/comma_if.hpp>
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
            typedef
                ungrouped_args
            grouped_by_type;
        };

        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        struct property_traits<BOOST_PP_ENUM_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY), typename A), grouped_by<BOOST_PP_CAT(A, BOOST_PP_DEC(QCHK_MAX_ARITY))> >
        {
            typedef bool call_signature(
                BOOST_PP_ENUM_BINARY_PARAMS(
                    BOOST_PP_DEC(QCHK_MAX_ARITY)
                  , typename wrap_array<A, >::type BOOST_PP_INTERCEPT
                )
            );
            typedef typename
                fusion::result_of::make_vector<
                    BOOST_PP_ENUM_BINARY_PARAMS(
                        BOOST_PP_DEC(QCHK_MAX_ARITY)
                      , typename wrap_array<A, >::type BOOST_PP_INTERCEPT
                    )
                >::type
            args_type;
            typedef typename
                wrap_array<BOOST_PP_CAT(A, BOOST_PP_DEC(QCHK_MAX_ARITY))>::type
            grouped_by_type;
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
            typedef                                                                                 \
                ungrouped_args                                                                      \
            grouped_by_type;                                                                        \
        };                                                                                          \
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>                                               \
        struct property_traits<BOOST_PP_ENUM_PARAMS(BOOST_PP_DEC(N), A) BOOST_PP_COMMA_IF(BOOST_PP_DEC(N)) grouped_by<BOOST_PP_CAT(A, BOOST_PP_DEC(N))> > \
        {                                                                                           \
            typedef bool call_signature(                                                            \
                BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(N), typename wrap_array<A, >::type BOOST_PP_INTERCEPT)   \
            );                                                                                      \
            typedef typename                                                                        \
                fusion::result_of::make_vector<                                                     \
                    BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(N), typename wrap_array<A, >::type BOOST_PP_INTERCEPT) \
                >::type                                                                             \
            args_type;                                                                              \
            typedef typename                                                                        \
                wrap_array<BOOST_PP_CAT(A, BOOST_PP_DEC(N))>::type                                  \
            grouped_by_type;                                                                        \
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
        typedef detail::property_traits<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> property_traits_type;
        typedef typename property_traits_type::call_signature sig_type;
        typedef typename property_traits_type::args_type args_type;
        typedef typename property_traits_type::grouped_by_type grouped_by_type;

        boost::function<std::string(args_type &)> classifier_;
        boost::function<grouped_by_type(args_type &)> grouper_;
    public:
        template<typename Expr>
        property(Expr const &expr, typename boost::enable_if<proto::is_expr<Expr> >::type* = 0)
          : boost::function<sig_type>(detail::GetProperty()(expr))
          , classifier_(detail::GetClassifiers()(expr, detail::unclassified_args()))
          , grouper_(detail::GetGrouper()(expr))
        {}

        typedef bool result_type; // for TR1
        using boost::function<sig_type>::operator();

        typedef boost::function<std::string(args_type &)> classifier_type;
        classifier_type const &classifier() const
        {
            return this->classifier_;
        }

        typedef boost::function<grouped_by_type(args_type &)> grouper_type;
        grouper_type const &grouper() const
        {
            return this->grouper_;
        }
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
