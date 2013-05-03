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
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/support/void.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>

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

        qcheck_args(args_type const &args)
          : args_type(args)
        {}

        friend std::ostream &operator<<(std::ostream &sout, qcheck_args const &args)
        {
            bool first = true;
            sout << "(";
            fusion::for_each(args, detail::disp(sout, first));
            return sout << ")";
        }
    };

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_results
    {
    public:
        qcheck_results()
          : failures()
        {}

        bool success() const
        {
            return this->failures.empty();
        }

        std::vector<qcheck_args<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> > failures;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
