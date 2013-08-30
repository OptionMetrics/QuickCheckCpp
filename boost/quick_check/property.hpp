///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file property.hpp
/// \brief Definition of the \c quick_check::property\<\> template, for storing property
///        objects.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/quick_check/qcheck_results.hpp>
#include <boost/quick_check/detail/grammar.hpp>
#include <boost/quick_check/condition.hpp>
#include <boost/quick_check/classify.hpp>
#include <boost/quick_check/group_by.hpp>

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
        struct property_traits<
            BOOST_PP_ENUM_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY), A)
          , grouped_by<BOOST_PP_CAT(A, BOOST_PP_DEC(QCHK_MAX_ARITY))>
        >
        {
            typedef bool call_signature(
                BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY)
                                          , typename wrap_array<A, >::type BOOST_PP_INTERCEPT)
            );
            typedef typename
                fusion::result_of::make_vector<
                    BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(QCHK_MAX_ARITY)
                                              , typename wrap_array<A, >::type BOOST_PP_INTERCEPT)
                >::type
            args_type;
            typedef typename
                wrap_array<BOOST_PP_CAT(A, BOOST_PP_DEC(QCHK_MAX_ARITY))>::type
            grouped_by_type;
        };

        template<>
        struct property_traits<>
        {
            typedef bool call_signature();
            typedef fusion::vector0<> args_type;
            typedef ungrouped_args grouped_by_type;
        };

        /// INTERNAL ONLY
    #define BOOST_PP_LOCAL_MACRO(N)                                                                 \
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>                                               \
        struct property_traits<BOOST_PP_ENUM_PARAMS(N, A)>                                          \
        {                                                                                           \
            typedef bool call_signature(                                                            \
                BOOST_PP_ENUM_BINARY_PARAMS(N,                                                      \
                                            typename wrap_array<A, >::type BOOST_PP_INTERCEPT)      \
            );                                                                                      \
            typedef typename                                                                        \
                fusion::result_of::make_vector<                                                     \
                    BOOST_PP_ENUM_BINARY_PARAMS(N,                                                  \
                                                typename wrap_array<A, >::type BOOST_PP_INTERCEPT)  \
                >::type                                                                             \
            args_type;                                                                              \
            typedef                                                                                 \
                ungrouped_args                                                                      \
            grouped_by_type;                                                                        \
        };                                                                                          \
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>                                               \
        struct property_traits<                                                                     \
            BOOST_PP_ENUM_PARAMS(BOOST_PP_DEC(N), A)                                                \
            BOOST_PP_COMMA_IF(BOOST_PP_DEC(N))                                                      \
            grouped_by<BOOST_PP_CAT(A, BOOST_PP_DEC(N))>                                            \
        >                                                                                           \
        {                                                                                           \
            typedef bool call_signature(                                                            \
                BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(N),                                        \
                                            typename wrap_array<A, >::type BOOST_PP_INTERCEPT)      \
            );                                                                                      \
            typedef typename                                                                        \
                fusion::result_of::make_vector<                                                     \
                    BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_DEC(N),                                    \
                                                typename wrap_array<A, >::type BOOST_PP_INTERCEPT)  \
                >::type                                                                             \
            args_type;                                                                              \
            typedef typename                                                                        \
                wrap_array<BOOST_PP_CAT(A, BOOST_PP_DEC(N))>::type                                  \
            grouped_by_type;                                                                        \
        };                                                                                          \
        /**/

        /// INTERNAL ONLY
    #define BOOST_PP_LOCAL_LIMITS (1, BOOST_PP_DEC(QCHK_MAX_ARITY))
    #include BOOST_PP_LOCAL_ITERATE()
    }


#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
    /// \brief A holder for quickCheckCpp properties.
    ///
    /// \tparam As A variadic parameter list. The types in the list should
    ///            correspond to the argument types of the property; i.e. type
    ///            <tt>result_type</tt>s of the generators bound to the property's
    ///            placeholders. Additionally, if the property has a grouping
    ///            criterion (as specified with <tt>quick_check::group_by()</tt>),
    ///            then the final type in \c As should be \c grouped_by\<X\>,
    ///            where \c X is the type by which the intput will be grouped.
    ///
    /// You may use \c property\<\> to declare an object to hold a property. In
    /// most cases, use of \c property\<\> is unnecessary; rather, you can declare
    /// properties with \c auto. \c property\<\> is necessary when the language
    /// forces you to declare objects with a specific type; e.g. as a data member.
    ///
    /// 
    template<typename ...As>
    struct property
      : private boost::function<
            typename detail::property_traits<As...>::call_signature
        >
    {
    private:
        typedef detail::property_traits<As...> property_traits_type;
#else
    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct property
      : private boost::function<
            typename detail::property_traits<
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)
            >::call_signature
        >
    {
    private:
        typedef
            detail::property_traits<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>
        property_traits_type;
#endif
        typedef typename property_traits_type::call_signature sig_type;
        typedef typename property_traits_type::args_type args_type;
        typedef typename property_traits_type::grouped_by_type grouped_by_type;

        boost::function<std::vector<std::string>(args_type const &)> classifier_;
        boost::function<grouped_by_type(args_type const &)> grouper_;
        boost::function<bool(args_type const &)> condition_;

    public:
        property()
          : boost::function<sig_type>()
          , classifier_()
          , grouper_()
          , condition_()
        {}

        template<typename Expr>
        property(
            Expr const &expr
            QCHK_DOXY_HIDDEN(BOOST_PP_COMMA()
                             typename boost::enable_if<proto::is_expr<Expr> >::type* = 0)
        )
          : boost::function<sig_type>(detail::GetProperty()(expr))
          , classifier_(detail::GetClassifiers()(expr, detail::unclassified_args()))
          , grouper_(detail::GetGrouper()(expr))
          , condition_(detail::GetCondition()(expr))
        {}

        typedef bool result_type; // for TR1

        // hack to get doxygen to emit something useful here
#ifdef QCHK_DOXYGEN_INVOKED
        bool operator()(As const &... as) const { return true; }
#else
        using boost::function<sig_type>::operator();
#endif

        typedef
            QCHK_DOXY_DETAIL(boost::function<std::vector<std::string>(args_type const &)>)
        classifier_type;

        classifier_type const &classifier() const
        {
            return this->classifier_;
        }

        typedef
            QCHK_DOXY_DETAIL(boost::function<grouped_by_type(args_type const &)>)
        grouper_type;

        grouper_type const &grouper() const
        {
            return this->grouper_;
        }

        typedef
            QCHK_DOXY_DETAIL(boost::function<bool(args_type const &)>)
        condition_type;

        condition_type const &condition() const
        {
            return this->condition_;
        }
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
