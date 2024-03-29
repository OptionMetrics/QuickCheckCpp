////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file qcheck_results.hpp
/// \brief Definition of \c quick_check::qcheck_results\<\> object, returned by the
///        \c quick_check::qcheck() algorithm, and also the \c quick_check::qcheck_args\<\>
///        template.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: This library is not yet an official Boost library.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_QCHECK_RESULTS_HPP_INCLUDED
#define QCHK_QCHECK_RESULTS_HPP_INCLUDED

//#include <map>
#include <vector>
#include <iosfwd>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/move/utility.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/sequence/intrinsic/back.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/view/single_view.hpp>
#include <boost/fusion/functional/adapter/fused.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/detail/array.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/container/flat_map.hpp>

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

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
        template<typename Seq, template<typename...> class T>
        struct fanout_params
        {
            struct inner
            {
                template<typename Sig>
                struct result;

                template<typename This, typename ...U>
                struct result<This(U...)>
                {
                    typedef T<U...> type;
                };

                template<typename ...U>
                T<U...> operator()(U const &...) const
                {
                    return T<U...>();
                }
            };
            typedef
                typename boost::result_of<
                    boost::fusion::fused<inner>(Seq)
                >::type
            type;
        };
#else
        #define TYPENAME(Z,N,D) typename ///< INTERNAL ONLY
        #define AT_C(Z,N,D) typename safe_at_c<D, N>::type ///< INTERNAL ONLY
        template<typename Seq, template<BOOST_PP_ENUM(QCHK_MAX_ARITY, TYPENAME, ~)> class T>
        struct fanout_params
        {
            typedef T<BOOST_PP_ENUM(QCHK_MAX_ARITY, AT_C, Seq)> type;
        };
        #undef AT_C
        #undef TYPENAME
#endif

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

        template<>
        struct find_grouped_by_type<>
        {
            typedef ungrouped_args type;
        };
    }

    /// \brief A Fusion Random Access Sequence that holds a set of arguments,
    /// the classifications for that argument set (if any), and the group
    /// of the arguments (if any).
    ///
    /// \c qcheck_args<> is used by \c qcheck_results to store sets of arguments
    /// together with the metadata about them. The associated metadata includes
    /// the argument classifications and group.
    ///
    /// After a failed \c qcheck() test, the argument sets that caused failure
    /// are available via \c qcheck_results::failures(), which returns a Range
    /// of \c qcheck_args<> objects.
    ///
    /// Since \c qcheck_args<> is a Fusion Random Access Sequence, the elements
    /// can be accessed with \c boost::fusion::at_c().
    ///
    /// \sa \c qcheck_results::failures
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
    template<typename...As>
    struct qcheck_args
      : fusion::result_of::make_vector<
            typename detail::fusion_elem_ignore_grouped<As>::type...
        >::type
    {
        /// A tuple-like type where the tuple elements correspond the
        /// the template arguments with which this \c property<> type
        /// was instantiated, minus \c grouped_by<>, if present.
        typedef
            typename fusion::result_of::make_vector<
                typename detail::fusion_elem_ignore_grouped<As>::type...
            >::type
        args_type;

        /// If an instance of \c grouped_by<> is the last template parameter
        /// in <tt>As...</tt>, then \c grouped_by_type is the type with which
        /// \c grouped_by<> was instantiated. Otherwise, \c grouped_by_type
        /// is some unspecified type.
        typedef
            typename detail::find_grouped_by_type<As...>::type
        grouped_by_type;
#else
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
#endif

        /// Constructor
        ///
        /// \param args A tuple-like object containing a set of arguments.
        /// \param classes The classifications applicable to this set of arguments, if any.
        /// \param group The group to which this set of arguments belongs, if any.
        qcheck_args(
            args_type const &args
          , std::vector<std::string> const &classes
          , grouped_by_type const &group
        )
          : args_type(args)
          , classes_(classes)
          , group_(group)
        {}

        /// \overload
        ///
        qcheck_args(args_type const &args, std::vector<std::string> const &classes)
          : args_type(args)
          , classes_(classes)
          , group_()
        {}

        /// \overload
        ///
        explicit qcheck_args(args_type const &args)
          : args_type(args)
          , classes_()
          , group_()
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

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(QCHK_DOXYGEN_INVOKED)
    /// \brief Holds the results of \c qcheck() algorithm.
    ///
    /// \tparam As A parameter pack that represents the types of the arguments
    ///            of the property that was tested, and (optionally), the result
    ///            type of the property's \c group_by() clause.
    ///
    /// The \c qcheck() algorithm records its results in an object of type
    /// \c qcheck_results<>, which it returns. \c qcheck_results<> remembers the
    /// argument sets which caused the test to fail. 
    ///
    /// \c qcheck_results<> also remembers metadata about the full test run, such
    /// as which input categories and groups were seen and with what frequency. This
    /// information can be displayed with the \c qcheck_results::print_summary()
    /// function.
    template<typename ...As>
    struct qcheck_results
    {
    public:
        typedef qcheck_args<As...> args_type;
        typedef
            typename detail::find_grouped_by_type<As...>::type
        grouped_by_type;
#else
    template<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, typename A)>
    struct qcheck_results
    {
    public:
        typedef qcheck_args<BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)> args_type;
        typedef
            typename detail::find_grouped_by_type<
                BOOST_PP_ENUM_PARAMS(QCHK_MAX_ARITY, A)
            >::type
        grouped_by_type;
#endif
        typedef std::vector<args_type> failures_type;
    private:
        QCHK_DOXY_HIDDEN(
            struct detail_smart_bool_type_
            {
                int m_;
            };
        )
        typedef
            std::pair<grouped_by_type, std::vector<std::string> >
        key_type;

        BOOST_COPYABLE_AND_MOVABLE(qcheck_results)

    public:
        /// Default constructor
        ///
        /// \post <tt>!this->exhausted()</tt>
        /// \post <tt>this->success()</tt>
        /// \post <tt>this->failures().empty()</tt>
        qcheck_results()
          : failures_()
          , categories_()
          , nbr_tests_(0)
          , first_failed_test_(0)
          , exhausted_(false)
        {}

        /// Copy constructor
        ///
        /// \param that The object to copy
        ///
        /// \post <tt>this->exhausted() == that.exhausted()</tt>
        /// \post <tt>this->success() == that.success()</tt>
        /// \post <tt>this->failures() == that.failures()</tt>
        qcheck_results(qcheck_results const &that)
          : failures_(that.failures_)
          , categories_(that.categories_)
          , nbr_tests_(that.nbr_tests_)
          , first_failed_test_(that.first_failed_test_)
          , exhausted_(that.exhausted_)
        {}

        /// Copy assignment operator
        ///
        /// \param that The object to copy
        ///
        /// \post <tt>this->exhausted() == that.exhausted()</tt>
        /// \post <tt>this->success() == that.success()</tt>
        /// \post <tt>this->failures() == that.failures()</tt>
        qcheck_results &operator=(BOOST_COPY_ASSIGN_REF(qcheck_results) that)
        {
            this->failures_ = that.failures_;
            this->categories_ = that.categories_;
            this->nbr_tests_ = that.nbr_tests_;
            this->first_failed_test_ = that.first_failed_test_;
            this->exhausted_ = that.exhausted_;
            return *this;
        }

        /// Move constructor
        ///
        /// \throw nothrow
        ///
        /// \param that The object to move from
        ///
        /// \c *this receives \c that's state, and \c that is left in an
        /// unspecified state.
        qcheck_results(BOOST_RV_REF(qcheck_results) that) BOOST_NOEXCEPT
          : failures_(boost::move(that.failures_))
          , categories_(boost::move(that.categories_))
          , nbr_tests_(that.nbr_tests_)
          , first_failed_test_(that.first_failed_test_)
          , exhausted_(that.exhausted_)
        {}

        /// Move assignment operator
        ///
        /// \throw nothrow
        ///
        /// \param that The object to move from
        ///
        /// \c *this receives \c that's state, and \c that is left in an
        /// unspecified state.
        qcheck_results &operator=(BOOST_RV_REF(qcheck_results) that) BOOST_NOEXCEPT
        {
            this->failures_ = boost::move(that.failures_);
            this->categories_ = boost::move(that.categories_);
            this->nbr_tests_ = that.nbr_tests_;
            this->first_failed_test_ = that.first_failed_test_;
            this->exhausted_ = that.exhausted_;
            return *this;
        }

        /// If \c *this was returned from \c qcheck(), then \c success() reports
        /// the success or failure of the \c qcheck() test run. Otherwise, returns
        /// true.
        ///
        /// \return <tt>this->failures().empty()</tt>
        ///
        /// \throw nothrow
        bool success() const
        {
            return this->failures_.empty();
        }

        /// If \c *this was returned from \c qcheck(), then \c exhausted() reports
        /// true if \c max_test_count was reached before \c test_count and false
        /// otherwise.
        ///
        /// \return true or false depending on whether the test ran out of input
        ///         before completing.
        ///
        /// \throw nothrow
        ///
        /// \sa \c config::test_count
        /// \sa \c config::max_test_count
        bool exhausted() const
        {
            return this->exhausted_;
        }

        /// INTERNAL ONLY
        typedef int detail_smart_bool_type_::* unspecified_bool_type;

        /// Conversion to a Boolean value
        ///
        /// \return A value convertible to true if <tt>this->success()</tt> is
        /// true; false otherwise.
        ///
        /// \throw nothrow
        operator unspecified_bool_type() const
        {
            return this->success() ? &detail_smart_bool_type_::m_ : 0;
        }

        /// Returns a Range of arguments and their associated metadata that caused
        /// an invocation of the \c qcheck() algorithm to fail.
        ///
        std::vector<args_type> const &failures() const
        {
            return this->failures_;
        }

        /// Write a summary of the results of the \c qcheck() algorithm to the
        /// specified \c std::ostream.
        ///
        /// \param sout The \c std::ostream to which the summary should be written.
        ///
        /// \return \c sout
        std::ostream &print_summary(std::ostream &sout = std::cout) const
        {
            if(this->success())
            {
                if(this->exhausted_)
                    sout << (boost::format("Arguments exhausted after %1% tests.\n")
                                % this->nbr_tests_);
                else
                    sout << (boost::format("OK, passed %1% tests.\n")
                                % this->nbr_tests_);
                // If we have groups or classes, print them now.
                if(!std::is_same<grouped_by_type, detail::ungrouped_args>::value ||
                   this->categories_.size() != 1 ||
                   this->categories_.begin()->first.second.size() != 0)
                {
                    // Iterate over the categories and print them with percentages
                    for(std::pair<key_type, std::size_t> const &p : this->categories_)
                    {
                        sout << (boost::format("%1$.0f%% %2%.\n")
                                    % ((p.second * 100.) / this->nbr_tests_)
                                    % this->category_name(p.first));
                    }
                    sout << std::flush;
                }
            }
            else
            {
                sout << (boost::format("Falsifiable, after %1% tests:\n%2%\n")
                            % this->first_failed_test_
                            % this->failures_[0]);
                sout << std::flush;
            }
            return sout;
        }

    private:
        /// INTERNAL ONLY
        static std::string category_name(key_type const &p)
        {
            std::stringstream sout;
            std::vector<std::string> tmp = p.second;
            if(!std::is_same<grouped_by_type, detail::ungrouped_args>::value)
                tmp.insert(tmp.begin(), boost::lexical_cast<std::string>(p.first));
            BOOST_ASSERT(!tmp.empty());
            bool first = true;
            std::for_each(tmp.begin(), tmp.end(), detail::disp(sout, first));
            return sout.str();
        }

        /// INTERNAL ONLY
        typedef typename args_type::args_type inner_args_type;

        /// INTERNAL ONLY
        void add_failure(
            inner_args_type const &args
          , std::vector<std::string> const &classes
          , grouped_by_type const &group
        )
        {
            this->add_success(classes, group);
            failures_.push_back(args_type(args, classes, group));
            if(this->first_failed_test_ == 0)
                this->first_failed_test_ = this->nbr_tests_;
        }

        /// INTERNAL ONLY
        void add_success(
            std::vector<std::string> const &classes
          , grouped_by_type const &group
        )
        {
            // Track the group/category as long as there is one.
            if(!std::is_same<grouped_by_type, detail::ungrouped_args>::value ||
               !classes.empty())
                ++this->categories_[key_type(group, classes)];
            ++this->nbr_tests_;
        }

        /// INTERNAL ONLY
        void set_exhausted()
        {
            this->exhausted_ = true;
        }

        friend struct detail::qcheck_access;
        std::vector<args_type> failures_;
        boost::container::flat_map<key_type, std::size_t> categories_;
        std::size_t nbr_tests_;
        std::size_t first_failed_test_;
        bool exhausted_;
    };

}

QCHK_BOOST_NAMESPACE_END

#endif
