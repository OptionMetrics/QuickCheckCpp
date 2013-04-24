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
#include <boost/quick_check/quick_check_fwd.hpp>

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
        private:
            std::ostream &sout_;
            bool &first_;
        };
    }

    template<typename Args>
    struct qcheck_args
      : Args
    {
        qcheck_args(Args const &args)
          : Args(args)
        {}

        friend std::ostream &operator<<(std::ostream &sout, qcheck_args const &args)
        {
            bool first = true;
            sout << "(";
            fusion::for_each(args, detail::disp(sout, first));
            return sout << ")";
        }
    };

    template<typename Args>
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

        std::vector<qcheck_args<Args> > failures;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
