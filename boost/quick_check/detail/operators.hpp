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
        struct unary_plus : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                + static_cast<T &&>(t)
            )
        };

        /// Function object for the unary - operator.
        struct negate : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                - static_cast<T &&>(t)
            )
        };

        /// Function object for the unary * operator.
        struct dereference : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                * static_cast<T &&>(t)
            )
        };

        /// Function object for the unary ~ operator.
        struct complement : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ~ static_cast<T &&>(t)
            )
        };

        /// Function object for the unary & operator.
        struct address_of : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                & static_cast<T &&>(t)
            )
        };

        /// Function object for the unary ! operator.
        struct logical_not : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ! static_cast<T &&>(t)
            )
        };

        /// Function object for the unary prefix ++ operator.
        struct pre_inc : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                ++ static_cast<T &&>(t)
            )
        };

        /// Function object for the unary prefix -- operator.
        struct pre_dec : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                -- static_cast<T &&>(t)
            )
        };

        /// Function object for the unary postfix ++ operator.
        struct post_inc : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                static_cast<T &&>(t) ++
            )
        };

        /// Function object for the unary postfix -- operator.
        struct post_dec : unary<>
        {
            template<typename T>
            BOOST_CONSTEXPR auto operator()(T && t) const
            QCHK_RETURN(
                static_cast<T &&>(t) --
            )
        };

        /// Function object for the binary \<\< operator.
        struct shift_left : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) << static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>\> operator.
        struct shift_right : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >> static_cast<U &&>(u)
            )
        };

        /// Function object for the binary * operator.
        struct multiplies : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) * static_cast<U &&>(u)
            )
        };

        /// Function object for the binary / operator.
        struct divides : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) / static_cast<U &&>(u)
            )
        };

        /// Function object for the binary % operator.
        struct modulus : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) % static_cast<U &&>(u)
            )
        };

        /// Function object for the binary + operator.
        struct plus : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) + static_cast<U &&>(u)
            )
        };

        /// Function object for the binary - operator.
        struct minus : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) - static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \< operator.
        struct less : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) < static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \> operator.
        struct greater : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) > static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \<= operator.
        struct less_equal : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) <= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>= operator.
        struct greater_equal : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary == operator.
        struct equal_to : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) == static_cast<U &&>(u)
            )
        };

        /// Function object for the binary != operator.
        struct not_equal_to : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) != static_cast<U &&>(u)
            )
        };

        /// Function object for the binary || operator.
        struct logical_or : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) || static_cast<U &&>(u)
            )
        };

        /// Function object for the binary && operator.
        struct logical_and : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) && static_cast<U &&>(u)
            )
        };

        /// Function object for the binary & operator.
        struct bitwise_and : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) & static_cast<U &&>(u)
            )
        };

        /// Function object for the binary | operator.
        struct bitwise_or : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) | static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ^ operator.
        struct bitwise_xor : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) ^ static_cast<U &&>(u)
            )
        };

        /// Function object for the binary , operator.
        struct comma : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) , static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ->* operator.
        struct mem_ptr : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) ->* static_cast<U &&>(u)
            )
        };

        /// Function object for the binary = operator.
        struct assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) = static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \<\<= operator.
        struct shift_left_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) <<= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary \>\>= operator.
        struct shift_right_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) >>= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary *= operator.
        struct multiplies_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) *= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary /= operator.
        struct divides_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) /= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary %= operator.
        struct modulus_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) %= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary += operator.
        struct plus_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) += static_cast<U &&>(u)
            )
        };

        /// Function object for the binary -= operator.
        struct minus_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) -= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary &= operator.
        struct bitwise_and_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) &= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary |= operator.
        struct bitwise_or_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) |= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary ^= operator.
        struct bitwise_xor_assign : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) -= static_cast<U &&>(u)
            )
        };

        /// Function object for the binary subscript operator.
        struct subscript : binary<>
        {
            template<typename T, typename U>
            BOOST_CONSTEXPR auto operator()(T && t, U && u) const
            QCHK_RETURN(
                static_cast<T &&>(t) [ static_cast<U &&>(u) ]
            )
        };

        /// Function object for the ternary ?: conditional operator.
        struct if_else : binary<>
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
