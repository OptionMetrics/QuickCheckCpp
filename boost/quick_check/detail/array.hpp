///////////////////////////////////////////////////////////////////////////////////////////////////
// \file array.hpp
// \brief Definition of some generic generators
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_ARRAY_HPP_INCLUDED
#define QCHK_DETAIL_ARRAY_HPP_INCLUDED

#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/array.hpp>
#include <boost/fusion/adapted/boost_array.hpp>
#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/quick_check/detail/operators.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct noop
        {
            typedef void result_type;
            template<typename T>
            void operator()(T &&) const
            {}
        };

        template<typename Seq, typename Fun>
        fusion::transform_view<Seq, Fun>
        make_transform_view(Seq &s, Fun f)
        {
            return fusion::transform_view<Seq, Fun>(s, f);
        }

        template<typename Seq0, typename Seq1, typename Fun>
        fusion::transform_view<Seq0, Seq1, Fun>
        make_transform_view(Seq0 &s0, Seq1 &s1, Fun f)
        {
            return fusion::transform_view<Seq0, Seq1, Fun>(s0, s1, f);
        }

        template<typename TN>
        struct array;

        template<typename T, std::size_t N>
        struct array<T[N]>
        {
            boost::array<T, N> elems;

            struct smart_bool_t { int m; };
            typedef int smart_bool_t::* unspecified_bool_type;
            operator unspecified_bool_type() const
            {
                struct and_ : std::unary_function<T, bool> {
                    bool operator()(bool b, T const&t) const {
                        return b && static_cast<bool>(t);
                    }
                };
                return fusion::fold(this->elems, true, and_()) ? &smart_bool_t::m : 0;
            }
        };

#define QCHK_UNARY_ARRAY_OP(OP, FUN)                                    \
        template<typename T, std::size_t N>                             \
        array<decltype(OP boost::declval<T>())[N]>                      \
        operator OP(array<T[N]> const &x)                               \
        {                                                               \
            array<decltype(OP boost::declval<T>())[N]> out;             \
            fusion::copy(                                               \
                make_transform_view(x.elems, FUN())                     \
              , out.elems                                               \
            );                                                          \
            return out;                                                 \
        }                                                               \
        /**/

#define QCHK_UNARY_ARRAY_PREXXX_OP(OP, FUN)                             \
        template<typename T, std::size_t N>                             \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x)                                     \
        {                                                               \
            fusion::for_each(x.elems, FUN());                           \
            return x;                                                   \
        }                                                               \
        /**/

#define QCHK_UNARY_ARRAY_POSTXXX_OP(OP, FUN)                            \
        template<typename T, std::size_t N>                             \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x, int)                                \
        {                                                               \
            array<T[N]> out = x;                                        \
            fusion::for_each(x.elems, FUN());                           \
            return out;                                                 \
        }                                                               \
        /**/

#define QCHK_BINARY_ARRAY_OP(OP, FUN)                                   \
        template<typename T, typename U, std::size_t N>                 \
        array<decltype(boost::declval<T>() OP boost::declval<U>())[N]>  \
        operator OP(array<T[N]> const &x, array<U[N]> const &y)         \
        {                                                               \
            array<                                                      \
                decltype(boost::declval<T>() OP boost::declval<U>())[N] \
            > out;                                                      \
            fusion::copy(                                               \
                make_transform_view(x.elems, y.elems, FUN())            \
              , out.elems                                               \
            );                                                          \
            return out;                                                 \
        }                                                               \
        /**/

#define QCHK_BINARY_ARRAY_ASSIGN_OP(OP, FUN)                            \
        template<typename T, typename U, std::size_t N>                 \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x, array<U[N]> const &y)               \
        {                                                               \
            fusion::for_each(                                           \
                make_transform_view(x.elems, y.elems, FUN())            \
              , noop()                                                  \
            );                                                          \
            return x;                                                   \
        }                                                               \
        /**/

        QCHK_UNARY_ARRAY_OP(+, unary_plus)
        QCHK_UNARY_ARRAY_OP(-, negate)
        QCHK_UNARY_ARRAY_OP(*, dereference)
        QCHK_UNARY_ARRAY_OP(~, complement)
        QCHK_UNARY_ARRAY_OP(!, logical_not)

        QCHK_UNARY_ARRAY_PREXXX_OP(++, pre_inc)
        QCHK_UNARY_ARRAY_PREXXX_OP(--, pre_dec)
        QCHK_UNARY_ARRAY_POSTXXX_OP(++, post_inc)
        QCHK_UNARY_ARRAY_POSTXXX_OP(--, post_dec)

        QCHK_BINARY_ARRAY_OP(<<, shift_left)
        QCHK_BINARY_ARRAY_OP(>>, shift_right)
        QCHK_BINARY_ARRAY_OP(*, multiplies)
        QCHK_BINARY_ARRAY_OP(/, divides)
        QCHK_BINARY_ARRAY_OP(%, modulus)
        QCHK_BINARY_ARRAY_OP(+, plus)
        QCHK_BINARY_ARRAY_OP(-, minus)
        QCHK_BINARY_ARRAY_OP(<, less)
        QCHK_BINARY_ARRAY_OP(>, greater)
        QCHK_BINARY_ARRAY_OP(<=, less_equal)
        QCHK_BINARY_ARRAY_OP(>=, greater_equal)
        QCHK_BINARY_ARRAY_OP(==, equal_to)
        QCHK_BINARY_ARRAY_OP(!=, not_equal_to)
        QCHK_BINARY_ARRAY_OP(||, logical_or)
        QCHK_BINARY_ARRAY_OP(&&, logical_and)
        QCHK_BINARY_ARRAY_OP(&, bitwise_and)
        QCHK_BINARY_ARRAY_OP(|, bitwise_or)
        QCHK_BINARY_ARRAY_OP(^, bitwise_xor)
        QCHK_BINARY_ARRAY_OP(BOOST_PP_COMMA(), comma)
        QCHK_BINARY_ARRAY_OP(->*, mem_ptr)

        QCHK_BINARY_ARRAY_ASSIGN_OP(<<=, shift_left_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(>>=, shift_right_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(*=, multiplies_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(/=, divides_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(%=, modulus_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(+=, plus_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(-=, minus_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(&=, bitwise_and_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(|=, bitwise_or_assign)
        QCHK_BINARY_ARRAY_ASSIGN_OP(^=, bitwise_xor_assign)
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
