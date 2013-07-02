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

#include <map>
#include <iosfwd>
#include <vector>
#include <sstream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/sequence/intrinsic/back.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/view/single_view.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/detail/array.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/repetition/enum.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename T>
        struct fusion_elem_ignore_grouped
          : fusion_elem<T>
        {};

        template<typename T>
        struct fusion_elem_ignore_grouped<grouped_by<T> >
        {
            typedef fusion::void_ type;
        };

        template<typename T>
        struct grouped_by_
        {
            typedef ungrouped_args type;
        };

        template<typename T>
        struct grouped_by_<grouped_by<T> >
        {
            typedef T type;
        };

        template<typename T, std::size_t N>
        struct grouped_by_<grouped_by<T[N]> >
        {
            typedef detail::array<T[N]> type;
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

        template<typename Args, typename GroupBy>
        struct make_qcheck_results_type
          : detail::fanout_params<
                typename fusion::result_of::as_vector<
                    typename fusion::result_of::join<
                        typename fusion::result_of::transform<
                            Args
                          , unpack_array
                        >::type
                      , fusion::single_view<grouped_by<GroupBy> >
                    >::type
                >::type
              , qcheck_results
            >
        {};

        template<typename Args, typename Array>
        struct make_qcheck_results_type<Args, detail::array<Array> >
          : detail::fanout_params<
                typename fusion::result_of::as_vector<
                    typename fusion::result_of::join<
                        typename fusion::result_of::transform<
                            Args
                          , unpack_array
                        >::type
                      , fusion::single_view<grouped_by<Array> >
                    >::type
                >::type
              , qcheck_results
            >
        {};

        template<typename Args>
        struct make_qcheck_results_type<Args, detail::ungrouped_args>
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

        template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(QCHK_MAX_ARITY, typename A, void)>
        struct find_grouped_by_type
          : detail::grouped_by_<
                typename remove_reference<
                    typename fusion::result_of::back<
                        typename fusion::result_of::make_vector<
                            BOOST_PP_ENUM_BINARY_PARAMS(QCHK_MAX_ARITY,
                                                        typename detail::fusion_elem<A,>::type
                                                        BOOST_PP_INTERCEPT)
                        >::type
                    >::type
                >::type
            >
        {};

    }

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_args
      : fusion::result_of::make_vector<
            BOOST_PP_ENUM_BINARY_PARAMS(QCHK_MAX_ARITY,
                                        typename detail::fusion_elem_ignore_grouped<A,>::type
                                        BOOST_PP_INTERCEPT)
        >::type
    {
        typedef
            typename fusion::result_of::make_vector<
                BOOST_PP_ENUM_BINARY_PARAMS(QCHK_MAX_ARITY,
                                            typename detail::fusion_elem_ignore_grouped<A,>::type
                                            BOOST_PP_INTERCEPT)
            >::type
        args_type;

        typedef
            typename detail::find_grouped_by_type<
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)
            >::type
        grouped_by_type;

        explicit qcheck_args(
            args_type const &args
          , std::vector<std::string> const &classes = std::vector<std::string>()
          , grouped_by_type const &group = grouped_by_type()
        )
          : args_type(args)
          , classes_(classes)
          , group_(group)
        {}

        friend std::ostream &operator<<(std::ostream &sout, qcheck_args const &args)
        {
            bool first = true;
            sout << "[";
            fusion::for_each(args, detail::disp(sout, first));
            sout << "]";
            if(!is_same<grouped_by_type, detail::ungrouped_args>::value)
            {
                sout << " (group: " << args.group_ << ")";
            }
            if(!args.classes_.empty())
            {
                first = true;
                sout << " (classes: ";
                boost::for_each(args.classes_, detail::disp(sout, first));
                sout << ")";
            }
            return sout;
        }

    private:
        std::vector<std::string> classes_;
        grouped_by_type group_;
    };

    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_results
    {
    public:
        typedef qcheck_args<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> args_type;
        typedef std::vector<args_type> failures_type;
        typedef
            typename detail::find_grouped_by_type<
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)
            >::type
        grouped_by_type;
    private:
        typedef
            std::pair<grouped_by_type, std::vector<std::string> >
        key_type;

    public:
        qcheck_results()
          : failures_()
          , categories_()
          , nbr_tests_(0)
          , first_failed_test_(0)
          , exhausted_(false)
        {}

        bool success() const
        {
            return this->failures_.empty();
        }

        failures_type const &failures() const
        {
            return this->failures_;
        }

        void print_summary(std::ostream &sout = std::cout) const
        {
            if(this->success())
            {
                if(this->exhausted_)
                    sout << (boost::format("Arguments exhausted after %1% tests.\n")
                                % this->nbr_tests_);
                else
                    sout << (boost::format("OK, passed %1% tests.\n")
                                % this->nbr_tests_);
                // If we have no groups and no classes, we're done.
                if(std::is_same<grouped_by_type, detail::ungrouped_args>::value &&
                   this->categories_.size() == 1 &&
                   this->categories_.begin()->first.second.size() == 0)
                    return;
                // Iterate over the categories and print them with percentages
                boost::for_each(
                    this->categories_
                  , [&](std::pair<key_type, std::size_t> const &p)
                    {
                        sout << (boost::format("%1$.0f%% %2%.\n")
                                    % ((p.second * 100.) / this->nbr_tests_)
                                    % this->category_name(p.first));
                    }
                );
                sout << std::flush;
            }
            else
            {
                sout << (boost::format("Falsifiable, after %1% tests:\n%2%\n")
                            % this->first_failed_test_
                            % this->failures_[0]);
                sout << std::flush;
            }
        }

    private:
        static std::string category_name(key_type const &p)
        {
            std::stringstream sout;
            std::vector<std::string> tmp = p.second;
            if(!std::is_same<grouped_by_type, detail::ungrouped_args>::value)
                tmp.insert(tmp.begin(), boost::lexical_cast<std::string>(p.first));
            bool first = true;
            std::for_each(tmp.begin(), tmp.end(), detail::disp(sout, first));
            return sout.str();
        }

        typedef typename args_type::args_type inner_args_type;

        void add_failure(
            inner_args_type const &args
          , std::vector<std::string> const &classes
          , grouped_by_type const &group
        )
        {
            failures_.push_back(args_type(args, classes, group));
            ++categories_[key_type(group, classes)];
            ++nbr_tests_;
            if(this->first_failed_test_ == 0)
                this->first_failed_test_ = this->nbr_tests_;
        }

        void add_success(
            std::vector<std::string> const &classes
          , grouped_by_type const &group
        )
        {
            ++this->categories_[key_type(group, classes)];
            ++this->nbr_tests_;
        }

        void exhausted()
        {
            this->exhausted_ = true;
        }

        friend struct detail::qcheck_access;
        failures_type failures_;
        std::map<key_type, std::size_t> categories_;
        std::size_t nbr_tests_;
        std::size_t first_failed_test_;
        bool exhausted_;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
