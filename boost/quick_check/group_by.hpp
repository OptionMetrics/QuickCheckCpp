///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file group_by.hpp
/// \brief Definition of the \c quick_check::group_by() function and the
///        \c quick_check::grouped_by\<\> template.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GROUP_BY_HPP_INCLUDED
#define QCHK_GROUP_BY_HPP_INCLUDED

#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/quick_check/detail/grammar.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct group_by_
        {};

        struct ungrouped_args
        {
            template<typename Args>
            ungrouped_args operator()(Args const &) const
            {
                return *this;
            }

            friend bool operator<(ungrouped_args, ungrouped_args)
            {
                return false;
            }

            friend std::ostream &operator<<(std::ostream &sout, ungrouped_args)
            {
                return sout;
            }
        };

        template<typename Fun>
        struct group_args
        {
            group_args(Fun const &fun)
              : fun_(fun)
            {}

            template<typename Args>
            typename fusion::result_of::invoke_function_object<Fun, Args>::type
            operator()(Args const &args) const
            {
                return fusion::invoke_function_object(this->fun_, args);
            }
        private:
            Fun fun_;
        };

        struct GetGrouperFromCombinators
          : proto::or_<
                proto::when<
                    Grouped
                  , group_args<boost::remove_reference<proto::_child> >(proto::_child)
                >
              , proto::when<
                    proto::bitwise_or<Grouped, Classify>
                  , group_args<boost::remove_reference<proto::_child(proto::_left)> >(proto::_child(proto::_left))
                >
              , proto::when<
                    proto::bitwise_or<GetGrouperFromCombinators, Classify>
                  , GetGrouperFromCombinators(proto::_left)
                >
            >
        {};

        struct GetGrouperFromCombinatorExpr
          : proto::or_<
                proto::when<
                    proto::bitwise_or<GetGrouperFromCombinators, phoenix::meta_grammar>
                  , GetGrouperFromCombinators(proto::_left)
                >
              , proto::when<
                    phoenix::meta_grammar
                  , proto::make<ungrouped_args>
                >
            >
        {};

        struct GetGrouperFromConditionalExpr
          : proto::when<
                proto::shift_right_assign<phoenix::meta_grammar, GetGrouperFromCombinatorExpr>
              , GetGrouperFromCombinatorExpr(proto::_right)
            >
        {};

        struct GetGrouper
          : proto::or_<
                GetGrouperFromConditionalExpr
              , GetGrouperFromCombinatorExpr
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<GetGrouper(Expr const &)>
        >::type
        get_grouper(Expr const & prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return GetGrouper()(prop);
        }

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
        template<typename...As>
        typename property<As...>::grouper_type const &
        get_grouper(property<As...> const &prop)
        {
            return prop.grouper();
        }
#else
        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        typename property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::grouper_type const &
        get_grouper(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop.grouper();
        }
#endif
    }

    /// \brief Used as the final template parameter to \c quick_check::property\<\>
    ///        to denote that the property's input arguments will be grouped by
    ///        objects of type \c T for reporting purposes.
    ///
    /// When defining a property that uses the \c quick_check::group_by() function
    /// to specify a grouping criterion, and assigning the property to a
    /// \c quick_check::property\<\> object, the \c property\<\> template must
    /// be parameterized on the type by to which the input will be grouped.
    /// The \c grouped_by\<\> template is the means by which \c property\<\> is
    /// parameterized.
    ///
    /// \em Example:
    ///
    /*! \code
        uniform<int> die(1,6);
        auto config = make_config(_1 = die);

        property<int, grouped_by<int> > prop =
            group_by(_1 % 3) | _1 >= 1 && _1 <= 6;
        \endcode
    */
    ///
    /// Since the property \c prop above has as its grouping criterion <tt>_1 % 3</tt>,
    /// and since \c _1 will be receiving objects of type \c int, the expression
    /// <tt>_1 % 3</tt> will yield <tt>int</tt>s. As a result, when declaring the type
    /// of \c prop, the final template parameter must be \c grouped_by\<int\>.
    ///
    /// \remark When declaring property objects with \c auto instead of with
    ///         \c property\<\>, \c grouped_by\<\> is unnecessary.
    ///
    /// The \c grouped_by\<\> template is also the final template parameter of any
    /// \c quick_check::qcheck_results\<\> objects returned by \c quick_check::qcheck()
    /// when used with a property defined with \c group_by().
    template<typename T>
    struct grouped_by
    {
        typedef T type;
    };

    /// \brief Used when defining properties to denote how the property's input
    ///        arguments should be grouped for reporting purposes.
    ///
    /// Use \c group_by() when defining a property to specify a grouping criterion.
    /// The summary results of the \c quick_check::qcheck() algorithm will contain
    /// the percentage of input parameters that are in each group, as defined by
    /// the criterion. The result of the grouping criterion will also be saved
    /// along with any input arguments that cause the property test to fail. 
    ///
    /// There can be only one \c group_by() clause in a property definition. It
    /// must follow the conditional clause (if any), and preceed the classification
    /// clauses (if any).
    ///
    /// \em Example:
    ///
    /*! \code
        uniform<int> die(1,6);
        auto config = make_config(_1 = die);

        auto prop =
            group_by(_1 % 3) | _1 >= 1 && _1 <= 6;
        \endcode
    */
    ///
    /// The use of \c group_by() in the above property definition signifies to the
    /// quick_check::qcheck() algorithm that it should collect statistics about how
    /// often the result of the expression <tt>_1 % 3</tt> is seen in the input.
    ///
    /// \em Example 2:
    ///
    /*! \code
        uniform<int> die(1,6);
        auto config = make_config(_1 = die);

        auto prop =
            (_1 != 1) >>=
                group_by(_1 % 3) |
                classify(_1 < 4, "small") |
                classify(_1 >= 4, "less small") |
                    _1 >= 2 && _1 <= 6;
        \endcode
    */
    ///
    /// The above property definition shows where the \c group_by() clause is allowed
    /// to appear relative to a conditional clause and the \c classify() clauses.
    template<typename Expr>
    typename proto::result_of::make_expr<
        detail::group_by_
      , detail::quick_check_domain
      , phoenix::actor<Expr>
    >::type const
    group_by(phoenix::actor<Expr> const &e)
    {
        return proto::make_expr<detail::group_by_, detail::quick_check_domain>(e);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
