///////////////////////////////////////////////////////////////////////////////////////////////////
// \file array.hpp
// \brief Definition of some generic generators
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_DETAIL_ARRAY_HPP_INCLUDED
#define QCHK_DETAIL_ARRAY_HPP_INCLUDED

#include <iosfwd>
#include <algorithm>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/array.hpp>
#include <boost/fusion/adapted/array.hpp>
#include <boost/fusion/adapted/boost_array.hpp>
#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/fusion/algorithm/auxiliary/copy.hpp>
#include <boost/fusion/sequence/comparison/equal_to.hpp>
#include <boost/fusion/sequence/comparison/not_equal_to.hpp>
#include <boost/fusion/sequence/comparison/greater.hpp>
#include <boost/fusion/sequence/comparison/greater_equal.hpp>
#include <boost/fusion/sequence/comparison/less.hpp>
#include <boost/fusion/sequence/comparison/less_equal.hpp>
#include <boost/quick_check/detail/operators.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        struct disp
        {
            disp(std::ostream &sout, bool &first)
              : sout_(sout)
              , first_(first)
            {}

            template<typename Value>
            void operator()(Value const &val) const
            {
                this->sout_ << (this->first_ ? "" : ", ") << val;
                this->first_ = false;
            }

            template<typename Value, std::size_t N>
            void operator()(boost::array<Value, N> const &rg) const
            {
                this->sout_ << (this->first_ ? "" : ",") << '[';
                bool first = true;
                std::for_each(rg.elems, rg.elems + N, disp(this->sout_, first));
                this->sout_ << ']';
                this->first_ = false;
            }
        private:
            std::ostream &sout_;
            bool &first_;
        };

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

        template<typename T>
        struct and_ : std::unary_function<T, bool>
        {
            bool operator()(bool b, T const&t) const
            {
                return b && static_cast<bool>(t);
            }
        };

        template<typename TN>
        struct array;

        template<typename T, std::size_t N>
        struct array<T[N]>
        {
            boost::array<T, N> elems;

            array()
            {}

            array(T const (&ar)[N])
            {
                std::copy(ar, ar + N, elems.elems);
            }

            array &operator=(array const &that)
            {
                elems = that.elems;
            }

            array &operator=(T (&ar)[N])
            {
                std::copy(ar, ar + N, elems.elems);
            }

            T &operator[](std::size_t n)
            {
                return elems.elems[n];
            }

            T const &operator[](std::size_t n) const
            {
                return elems.elems[n];
            }

            struct smart_bool_t { int m; };
            typedef int smart_bool_t::* unspecified_bool_type;
            operator unspecified_bool_type() const
            {
                return std::accumulate(elems.elems, elems.elems + N, true, and_<T>())
                    ? &smart_bool_t::m : 0;
            }

            friend std::ostream &operator<<(std::ostream &sout, array const &arr)
            {
                bool first = true;
                sout << "[";
                std::for_each(arr.elems.elems, arr.elems.elems + N, detail::disp(sout, first));
                return sout << "]";
            }
        };

#define QCHK_UNARY_ARRAY_OP(OP, FUN)                                    \
        template<typename T, std::size_t N>                             \
        array<decltype(OP boost::declval<T>())[N]>                      \
        operator OP(array<T[N]> const &x)                               \
        {                                                               \
            array<decltype(OP boost::declval<T>())[N]> out;             \
            std::transform(x.elems.elems, x.elems.elems + N, out.elems.elems, FUN()); \
            return out;                                                 \
        }                                                               \
        /**/

#define QCHK_UNARY_ARRAY_PREXXX_OP(OP, FUN)                             \
        template<typename T, std::size_t N>                             \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x)                                     \
        {                                                               \
            std::for_each(x.elems.elems, x.elems.elems + N, FUN());     \
            return x;                                                   \
        }                                                               \
        /**/

#define QCHK_UNARY_ARRAY_POSTXXX_OP(OP, FUN)                            \
        template<typename T, std::size_t N>                             \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x, int)                                \
        {                                                               \
            array<T[N]> out = x;                                        \
            std::for_each(x.elems.elems, x.elems.elems + N, FUN());     \
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
            std::transform(x.elems.elems, x.elems.elems + N, y.elems.elems, out.elems.elems, FUN());\
            return out;                                                 \
        }                                                               \
        template<typename T, typename U, std::size_t N>                 \
        array<decltype(boost::declval<T>() OP boost::declval<U>())[N]>  \
        operator OP(array<T[N]> const &x, U const (&y)[N])              \
        {                                                               \
            return x OP array<U[N]>(y);                                 \
        }                                                               \
        template<typename T, typename U, std::size_t N>                 \
        array<decltype(boost::declval<T>() OP boost::declval<U>())[N]>  \
        operator OP(T const (&x)[N], array<U[N]> const &y)              \
        {                                                               \
            return array<T[N]>(x) OP y;                                 \
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
        template<typename T, typename U, std::size_t N>                 \
        array<T[N]> &                                                   \
        operator OP(array<T[N]> &x, U const (&y)[N])                    \
        {                                                               \
            return x OP array<U[N]>(y);                                 \
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

#define QCHK_BINARY_ARRAY_REL_OP(OP, FUN)                                                           \
        template<typename T, typename U, std::size_t N>                                             \
        bool operator OP(array<T[N]> const &x, array<U[N]> const &y)                                \
        {                                                                                           \
            return fusion::FUN(x.elems, y.elems);                                                   \
        }                                                                                           \
        template<typename T, typename U, std::size_t N>                                             \
        bool operator OP(array<T[N]> const &x, U const (&y)[N])                                     \
        {                                                                                           \
            return fusion::FUN(x.elems, y);                                                         \
        }                                                                                           \
        template<typename T, typename U, std::size_t N>                                             \
        bool operator OP(T const (&x)[N], array<U[N]> const &y)                                     \
        {                                                                                           \
            return fusion::FUN(x, y.elems);                                                         \
        }                                                                                           \
        /**/

        QCHK_BINARY_ARRAY_REL_OP(<, less)
        QCHK_BINARY_ARRAY_REL_OP(>, greater)
        QCHK_BINARY_ARRAY_REL_OP(<=, less_equal)
        QCHK_BINARY_ARRAY_REL_OP(>=, greater_equal)
        QCHK_BINARY_ARRAY_REL_OP(==, equal_to)
        QCHK_BINARY_ARRAY_REL_OP(!=, not_equal_to)

        struct unpack_array
        {
            template<typename Sig>
            struct result
            {};

            template<typename This, typename T>
            struct result<This(T)>
            {
                typedef T type;
            };

            template<typename This, typename T, std::size_t N>
            struct result<This(detail::array<T[N]>)>
            {
                typedef boost::array<T, N> type;
            };

            template<typename This, typename T>
            struct result<This(T &)>
              : result<This(T)>
            {};

            template<typename This, typename T>
            struct result<This(T const &)>
              : result<This(T)>
            {};

            template<typename T>
            T operator()(T const &t) const
            {
                return t;
            }

            template<typename T, std::size_t N>
            boost::array<T, N> operator()(detail::array<T[N]> const &rg) const
            {
                return rg.elems;
            }
        };
    }
}

QCHK_BOOST_NAMESPACE_END

#endif
