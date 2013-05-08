////////////////////////////////////////////////////////////////////////////////////////////////////
// \file qcheck_results.hpp
// \brief Definition of qcheck_results object
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QCHECK_RESULTS_HPP_INCLUDED
#define QCHK_QCHECK_RESULTS_HPP_INCLUDED

#include <iosfwd>
#include <vector>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/support/void.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/repetition/enum.hpp>

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
                this->sout_ << (this->first_ ? "" : ",") << val;
                this->first_ = false;
            }

            template<typename Value, std::size_t N>
            void operator()(boost::array<Value, N> const &rg) const
            {
                this->sout_ << (this->first_ ? "" : ",") << '{';
                bool first = true;
                fusion::for_each(rg, disp(this->sout_, first));
                this->sout_ << '}';
                this->first_ = false;
            }
        private:
            std::ostream &sout_;
            bool &first_;
        };

        template<typename T>
        struct fusion_elem
        {
            typedef T type;
        };

        template<>
        struct fusion_elem<void>
        {
            typedef fusion::void_ type;
        };

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

        template<typename Seq, std::size_t I>
        struct safe_at_c
          : mpl::eval_if_c<
                I >= fusion::result_of::size<Seq>::value
              , mpl::identity<void>
              , fusion::result_of::value_at_c<Seq, I>
            >
        {};

        #define TYPENAME(Z,N,D) typename
        #define AT_C(Z,N,D) typename safe_at_c<D, N>::type
        template<typename Seq, template<BOOST_PP_ENUM(QCHK_MAX_ARITY, TYPENAME, ~)> class T>
        struct fanout_params
        {
            typedef T<BOOST_PP_ENUM(QCHK_MAX_ARITY, AT_C, Seq)> type;
        };
        #undef AT_C
        #undef TYPENAME

        template<typename Args>
        struct make_qcheck_results_type
          : detail::fanout_params<
                typename fusion::result_of::as_vector<
                    typename fusion::result_of::transform<
                        Args
                      , unpack_array
                    >::type
                >::type
              , qcheck_results
            >
        {};

    }

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_args
      : fusion::result_of::make_vector<
            BOOST_PP_ENUM_BINARY_PARAMS(QCHK_MAX_ARITY,
                                        typename detail::fusion_elem<A,>::type
                                        BOOST_PP_INTERCEPT)
        >::type
    {
        typedef
            typename fusion::result_of::make_vector<
                BOOST_PP_ENUM_BINARY_PARAMS(QCHK_MAX_ARITY,
                                            typename detail::fusion_elem<A,>::type
                                            BOOST_PP_INTERCEPT)
            >::type
        args_type;

        explicit qcheck_args(args_type const &args, std::string const &classname = std::string())
          : args_type(args)
          , classname_(classname)
        {}

        friend std::ostream &operator<<(std::ostream &sout, qcheck_args const &args)
        {
            bool first = true;
            sout << "(";
            fusion::for_each(args, detail::disp(sout, first));
            sout << ")";
            if(!args.classname_.empty())
                sout << " (class: " << args.classname_ << ")";
            return sout;
        }
    private:
        std::string classname_;
    };

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_results
    {
    public:
        typedef qcheck_args<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> args_type;
        typedef std::vector<args_type> failures_type;

        qcheck_results()
          : failures_()
        {}

        bool success() const
        {
            return this->failures_.empty();
        }

        failures_type const &failures() const
        {
            return this->failures_;
        }

    private:
        friend struct detail::qcheck_access;
        failures_type failures_;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
