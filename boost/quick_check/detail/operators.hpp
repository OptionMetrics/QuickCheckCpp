///////////////////////////////////////////////////////////////////////////////////////////////////
// \file operators.hpp
// \brief Operator function objects
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_OPERATORS_HPP_INCLUDED
#define QCHK_DETAIL_OPERATORS_HPP_INCLUDED

#include <boost/utility/declval.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/detail/functional.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        /// Function object for the unary + operator.
        struct unary_plus : unary<unary_plus>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                + static_cast<T &&>(t)
            )
        };

        /// Function object for the unary - operator.
        struct negate : unary<negate>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                - static_cast<T &&>(t)
            )
        };

        /// Function object for the unary * operator.
        struct dereference : unary<dereference>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                * static_cast<T &&>(t)
            )
        };

        /// Function object for the unary ~ operator.
        struct complement : unary<complement>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ~ static_cast<T &&>(t)
            )
        };

        /// Function object for the unary & operator.
        struct address_of : unary<address_of>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                & static_cast<T &&>(t)
            )
        };

        /// Function object for the unary ! operator.
        struct logical_not : unary<logical_not>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ! static_cast<T &&>(t)
            )
        };

        /// Function object for the unary prefix ++ operator.
        struct pre_inc : unary<pre_inc>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ++ static_cast<T &&>(t)
            )
        };

        /// Function object for the unary prefix -- operator.
        struct pre_dec : unary<pre_dec>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                -- static_cast<T &&>(t)
            )
        };

        /// Function object for the unary postfix ++ operator.
        struct post_inc : unary<post_inc>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                static_cast<T &&>(t) ++
            )
        };

        /// Function object for the unary postfix -- operator.
        struct post_dec : unary<post_dec>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                static_cast<T &&>(t) --
            )
        };

        /// Function object for the binary \<\< operator.
        struct shift_left : binary<shift_left>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) << static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>\> operator.
        struct shift_right : binary<shift_right>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >> static_cast<U &&>(u)
            )
        };

        /// Function object for the binary * operator.
        struct multiplies : binary<multiplies>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) * static_cast<U &&>(u)
            )
        };

        /// Function object for the binary / operator.
        struct divides : binary<divides>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) / static_cast<U &&>(u)
            )
        };

        /// Function object for the binary % operator.
        struct modulus : binary<modulus>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) % static_cast<U &&>(u)
            )
        };

        /// Function object for the binary + operator.
        struct plus : binary<plus>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) + static_cast<U &&>(u)
            )
        };

        /// Function object for the binary - operator.
        struct minus : binary<minus>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) - static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \< operator.
        struct less : binary<less>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) < static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \> operator.
        struct greater : binary<greater>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) > static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \<= operator.
        struct less_equal : binary<less_equal>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) <= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>= operator.
        struct greater_equal : binary<greater_equal>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary == operator.
        struct equal_to : binary<equal_to>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) == static_cast<U &&>(u)
            )
        };

        /// Function object for the binary != operator.
        struct not_equal_to : binary<not_equal_to>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) != static_cast<U &&>(u)
            )
        };

        /// Function object for the binary || operator.
        struct logical_or : binary<logical_or>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) || static_cast<U &&>(u)
            )
        };

        /// Function object for the binary && operator.
        struct logical_and : binary<logical_and>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) && static_cast<U &&>(u)
            )
        };

        /// Function object for the binary & operator.
        struct bitwise_and : binary<bitwise_and>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) & static_cast<U &&>(u)
            )
        };

        /// Function object for the binary | operator.
        struct bitwise_or : binary<bitwise_or>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) | static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ^ operator.
        struct bitwise_xor : binary<bitwise_xor>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) ^ static_cast<U &&>(u)
            )
        };

        /// Function object for the binary , operator.
        struct comma : binary<comma>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) , static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ->* operator.
        struct mem_ptr : binary<mem_ptr>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) ->* static_cast<U &&>(u)
            )
        };

        /// Function object for the binary = operator.
        struct assign : binary<assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) = static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \<\<= operator.
        struct shift_left_assign : binary<shift_left_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) <<= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>\>= operator.
        struct shift_right_assign : binary<shift_right_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >>= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary *= operator.
        struct multiplies_assign : binary<multiplies_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) *= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary /= operator.
        struct divides_assign : binary<divides_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) /= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary %= operator.
        struct modulus_assign : binary<modulus_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) %= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary += operator.
        struct plus_assign : binary<plus_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) += static_cast<U &&>(u)
            )
        };

        /// Function object for the binary -= operator.
        struct minus_assign : binary<minus_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) -= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary &= operator.
        struct bitwise_and_assign : binary<bitwise_and_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) &= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary |= operator.
        struct bitwise_or_assign : binary<bitwise_or_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) |= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ^= operator.
        struct bitwise_xor_assign : binary<bitwise_xor_assign>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) -= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary subscript operator.
        struct subscript : binary<subscript>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) [ static_cast<U &&>(u) ]
            )
        };

        /// Function object for the ternary ?: conditional operator.
        struct if_else : binary<if_else>
        {
            template<typename T, typename U, typename V>
            BOOST_CONSTEXPR auto operator()(T && t, U && u, V && v) const
            QCHK_RETURN(
                static_cast<T &&>(t) ? static_cast<U &&>(u) : static_cast<V &&>(v)
            )
        };

        ///// Function object for the n-ary function call operator.
        //struct function
        //{
        //    template<typename Fun, typename ...T>
        //    BOOST_CONSTEXPR auto operator()(Fun && fun, T &&... t) const
        //    QCHK_RETURN(
        //        static_cast<Fun &&>(fun)(static_cast<T &&>(t)...)
        //    )
        //};
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
