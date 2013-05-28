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
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/support/void.hpp>
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
        struct fusion_elem
        {
            typedef T type;
        };

        template<>
        struct fusion_elem<void>
        {
            typedef fusion::void_ type;
        };

        template<typename T>
        struct fusion_elem<grouped_by<T> >
        {
            typedef fusion::void_ type;
        };

        template<typename T>
        struct fusion_elem2
          : mpl::if_<is_void<T>, fusion::void_, T>
        {};

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
                                                        typename detail::fusion_elem2<A,>::type
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

        typedef
            typename detail::find_grouped_by_type<
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)
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
                std::for_each(args.classes_.begin(), args.classes_.end(), detail::disp(sout, first));
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
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)
            >::type
        grouped_by_type;

        qcheck_results()
          : failures_()
          , categories_()
          , nbr_tests_(0)
          , first_failed_test_(0)
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
                sout << boost::format("OK, passed %1% tests.\n") % this->nbr_tests_;
                // If we have no groups and no classes, we're done.
                if(this->categories_.size() == 1 &&
                   std::is_same<grouped_by_type, detail::ungrouped_args>::value &&
                   this->categories_.begin()->first.second.size() == 0)
                    return;
                // Iterate over the categories and print them with percentages
                for(auto const &p : this->categories_)
                {
                    sout << (boost::format("%1$.0d%% %2%.")
                                % (((double)p.second)/((double)this->nbr_tests_) * 100.)
                                % category_name(p.first))
                         << std::endl;
                }
            }
            else
            {
                sout << (boost::format("Falsifiable, after %1% tests:\n%2%")
                            % this->first_failed_test_
                            % this->failures_[0])
                     << std::endl;
            }
        }

    private:
        typedef
            std::pair<grouped_by_type, std::vector<std::string> >
        key_type;

        struct comparator
          : std::unary_function<key_type, bool>
        {
	        bool operator()(key_type const &left, key_type const &right) const
	        {
	            return (*this)(left.first, right.first) ||
		            !(*this)(right.first, right.first) && (*this)(left.second, left.second);
	        }

        private:
            template<typename T>
	        bool operator()(T const &left, T const &right) const
	        {
	            return left < right;
	        }

            template<typename T, std::size_t N>
	        bool operator()(detail::array<T[N]> const &left, detail::array<T[N]> const &right) const
	        {
	            return std::lexicographical_compare(
                    left.elems.elems, left.elems.elems + N,
                    right.elems.elems, right.elems.elems + N
                );
	        }
        };

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
            ++categories_[key_type(group, classes)];
            ++nbr_tests_;
        }

        friend struct detail::qcheck_access;
        failures_type failures_;
        std::map<key_type, std::size_t, comparator> categories_;
        std::size_t nbr_tests_;
        std::size_t first_failed_test_;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
