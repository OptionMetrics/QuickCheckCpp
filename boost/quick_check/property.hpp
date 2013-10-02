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
    /// \em Example:
    ///
    /*! \code
        uniform<int> di(1,6);
        normal<double> dd(0.0, 1.0);
        auto config = make_config(_1 = di, _2 = dd);

        // Notice how we assign a complicated property
        // expression to an instance of the property<>
        // class template. The arguments, int and double,
        // correspond to the types that the objects
        // di and dd generate. (The correspondence is
        // established in the line above.)
        property<int, double>
            is_reflexive =
                classify(_2>0.,"foo")
              | classify(_2<=0.,"bar")
              | ((_1 + _2) == (_2 + _1)) ;

        // Same as above, except with a group_by() clause.
        // The last template parameter specifies the type
        // by which arguments will be grouped.
        property<int, double, grouped_by<int>>
            is_reflexive_grouped =
                group_by(_1 % 3)
              | classify(_2>0.,"foo")
              | classify(_2<=0.,"bar")
              | ((_1 + _2) == (_2 + _1)) ;
        \endcode
    */
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
    public:
        /// A tuple-like type where the tuple elements correspond the
        /// the template arguments with which this \c property<> type
        /// was instantiated, minus \c grouped_by<>, if present.
        typedef
            QCHK_DOXY_DETAIL(typename property_traits_type::args_type)
        args_type;

        /// If an instance of \c grouped_by<> is the last template parameter
        /// in <tt>A...</tt>, then \c grouped_by_type is the type with which
        /// \c grouped_by<> was instantiated. Otherwise, \c grouped_by_type
        /// is some unspecified type.
        typedef
            QCHK_DOXY_DETAIL(typename property_traits_type::grouped_by_type)
        grouped_by_type;

    private:
        boost::function<std::vector<std::string>(args_type const &)> classifier_;
        boost::function<grouped_by_type(args_type const &)> grouper_;
        boost::function<bool(args_type const &)> condition_;

    public:
        /// Default-construct an empty property
        property()
          : boost::function<sig_type>()
          , classifier_()
          , grouper_()
          , condition_()
        {}

        /// Initialize a \c property<> object with a property expression.
        ///
        /// \note This constructor does not participate in overload resolution if
        /// \c Expr is not the type of a valid property expression.
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
        /// Evaluate this property with the specified arguments.
        ///
        /// \note <tt>As const &...</tt> here is equivalent to <tt>A const &...</tt>,
        /// with <tt>A...</tt> being the pack of template arguments used to instantiate
        /// \c property<>, with the exception of the final \c grouped_by<> argument,
        /// had it been specified. For instance, the arguments to
        /// <tt>property<int, double, grouped_by<int>>::operator()</tt> are
        /// \c int and \c double.
        ///
        /// \return True if this property holds for this set of arguments,
        /// false otherwise.
        bool operator()(As const &... as) const { return true; }
#else
        // This is what is actually used at runtime.
        using boost::function<sig_type>::operator();
#endif

        /// A model of
        /// <tt><a href="http://www.sgi.com/tech/stl/UnaryFunction.html">
        /// UnaryFunction</a></tt> whose argument type is
        /// \c args_type and whose result type is a model of
        /// <tt><a href="http://www.sgi.com/tech/stl/ForwardContainer.html">
        /// ForwardContainer</a></tt> whose \c value_type is \c std::string.
        typedef
            QCHK_DOXY_DETAIL(boost::function<std::vector<std::string>(args_type const &)>)
        classifier_type;

        /// \return A unary function corresponding to the \c classify()
        /// clauses of the property expression with which this \c property\<\>
        /// object was initialized, if any. If none was specified, the
        /// unary function returned by \c classifier() always returns an
        /// empty container.
        classifier_type const &classifier() const
        {
            return this->classifier_;
        }

        /// A model of
        /// <tt><a href="http://www.sgi.com/tech/stl/UnaryFunction.html">
        /// UnaryFunction</a></tt> whose argument type is
        /// \c args_type and whose result type is \c grouped_by_type.
        typedef
            QCHK_DOXY_DETAIL(boost::function<grouped_by_type(args_type const &)>)
        grouper_type;

        /// \return A unary function corresponding to the \c group_by()
        /// clause of the property expression with which this \c property\<\>
        /// object was initialized, if any.
        grouper_type const &grouper() const
        {
            return this->grouper_;
        }

        /// A model of
        /// <tt><a href="http://www.sgi.com/tech/stl/Predicate.html">
        /// Predicate</a></tt> whose argument type is
        /// \c args_type.
        typedef
            QCHK_DOXY_DETAIL(boost::function<bool(args_type const &)>)
        condition_type;

        /// \return A predicate corresponding to the condition clause
        /// of the property expression with which this \c property\<\>
        /// object was initialized, if one was specified. Otherwise, it
        /// returns a predicate that always returns true.
        condition_type const &condition() const
        {
            return this->condition_;
        }
    };
}

QCHK_BOOST_NAMESPACE_END

#endif
