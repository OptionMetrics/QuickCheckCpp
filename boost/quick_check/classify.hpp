///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file classify.hpp
/// \brief Definition of the \c quick_check::classify() function for declaring properties'
///        classification criteria.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_CLASSIFY_HPP_INCLUDED
#define QCHK_CLASSIFY_HPP_INCLUDED

#include <string>
#include <vector>
#include <algorithm>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/fusion/functional/invocation/invoke_function_object.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/quick_check/detail/grammar.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct classify_
        {};

        struct unclassified_args
        {
            typedef std::vector<std::string> result_type;

            template<typename Args>
            result_type operator()(Args &) const
            {
                return result_type();
            }
        };

        template<typename Cond, typename Rest = unclassified_args>
        struct classify_args
        {
            typedef std::vector<std::string> result_type;

            classify_args(Cond const &cond, std::string const &name, Rest const &rest = Rest())
              : cond_(cond)
              , name_(name)
              , rest_(rest)
            {}

            template<typename Args>
            result_type operator()(Args &args) const
            {
                result_type res = this->rest_(args);
                if(static_cast<bool>(fusion::invoke_function_object(this->cond_, args)))
                {
                    // Keep the vector sorted as we build it.
                    auto it = std::lower_bound(res.begin(), res.end(), this->name_);
                    res.insert(it, this->name_);
                }
                return res;
            }

        private:
            Cond cond_;
            std::string name_;
            Rest rest_;
        };

        struct make_classify_args : proto::callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Cond, typename Name, typename Rest>
            struct result<This(Cond, Name, Rest)>
            {
                typedef
                    classify_args<
                        typename boost::remove_const<typename remove_reference<Cond>::type>::type
                      , typename boost::remove_const<typename remove_reference<Rest>::type>::type
                    >
                type;
            };

            template<typename Cond, typename Rest>
            classify_args<Cond, Rest>
            operator()(Cond const &cond, std::string const &name, Rest const &rest) const
            {
                return classify_args<Cond, Rest>(cond, name, rest);
            }
        };

        struct GetClassifiers
          : proto::or_<
                proto::when<
                    Classify
                  , make_classify_args(
                        proto::_child0
                      , proto::_value(proto::_child1)
                      , proto::_state
                    )
                >
              , proto::when<
                    proto::terminal<proto::_>
                  , proto::_state
                >
              , proto::otherwise<
                    proto::fold<proto::_, proto::_state, GetClassifiers>
                >
            >
        {};

        template<typename Expr>
        typename boost::lazy_enable_if<
            proto::is_expr<Expr>
          , boost::result_of<GetClassifiers(Expr const &, unclassified_args const &)>
        >::type
        get_classifier(Expr const & prop)
        {
            static_assert(
                proto::matches<Expr, QuickCheckExpr>::value
              , "The specified quick-check expression does not match the grammar for "
                "valid quick-check expressions."
            );
            return GetClassifiers()(prop, unclassified_args());
        }

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
        template<typename ...As>
        typename property<As...>::classifier_type const &
        get_classifier(property<As...> const &prop)
        {
            return prop.classifier();
        }
#else
        template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
        typename property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)>::classifier_type const &
        get_classifier(property<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> const &prop)
        {
            return prop.classifier();
        }
#endif
    }

    /// \brief Associate input parameters that satisfy a predicate with a
    /// classification string for reporting later.
    ///
    /// \param e A Boolean lambda expression representing the predicate
    /// \param name The string representing the category name
    ///
    /// \return An object of unspecified type and value that may be combined
    /// with other classifiers into a QuickCheckExpression
    ///
    /// Input classifiers are used to verify that test coverage is as
    /// expected. The percentage of input that safisfies the classification
    /// criteria is reported at the end of test runs.
    ///
    /// Below is an example of a test that uses classification criteria
    /// to examine frequency of certain types of input:
    ///
    /*! \code
        uniform<int> di(1,6);
        normal<double> dd(0.0, 1.0);

        // a quickCheck configuration. Placeholders _1
        // and _2 receive values generated from di and dd:
        auto config = make_config(
            _1 = di,
            _2 = dd,
        );

        // Here's a property to test:
        auto
            is_reflexive =
                classify(_2>0.,"foo")
              | classify(_2<=0.,"bar")
              | ((_1 + _2) == (_2 + _1)) ;

        // Test the property. Res is a results object that
        // holds the values that caused the test to fail
        auto res = qcheck(is_reflexive, config);
        res.print_summary();
        \endcode
    */
    ///
    /// The code displays the following:
    ///
    /*! \code
        OK, passed 100 tests.
        51% bar.
        49% foo.
        \endcode
    */
    ///
    /// As in the above example, you may specify multiple classifiers.
    /// A given set of input parameters may satisfy more than one
    /// classification criterion; in that case, the class names of all
    /// the matching classification criteria are displayed.
    template<typename Expr>
    typename proto::result_of::make_expr<
        detail::classify_
      , detail::quick_check_domain
      , phoenix::actor<Expr>
      , std::string
    >::type const
    classify(phoenix::actor<Expr> const &e, std::string const& name)
    {
        return proto::make_expr<detail::classify_, detail::quick_check_domain>(e, name);
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
