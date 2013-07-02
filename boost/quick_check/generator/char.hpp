///////////////////////////////////////////////////////////////////////////////////////////////////
// \file char.hpp
// \brief Definition of char, for generating characters.
//
// Copyright 2013 OptionMetrics, Inc.
// Copyright 2013 Eric Niebler
//
// NOTE: This library is not yet an official Boost library.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef QCHK_GENERATOR_CHAR_HPP_INCLUDED
#define QCHK_GENERATOR_CHAR_HPP_INCLUDED

#include <climits>
#include <cctype>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/quick_check/quick_check_fwd.hpp>
#include <boost/quick_check/generator/basic_generator.hpp>
#include <boost/random/uniform_int_distribution.hpp>

QCHK_BOOST_NAMESPACE_BEGIN

namespace quick_check
{
    namespace detail
    {
        template<typename Char, typename UnaryPredicate>
        struct char_class_generator
        {
            typedef Char result_type;

            explicit char_class_generator(UnaryPredicate const &pred = UnaryPredicate())
              : dist_()
              , chars_()
            {
                static boost::int64_t const min = (std::numeric_limits<Char>::min)();
                static boost::int64_t const max = (std::numeric_limits<Char>::max)();
                chars_.reserve((1 << CHAR_BIT) - 1);
                for(boost::int64_t i = min; ; ++i)
                {
                    if(static_cast<bool>(pred((Char)i)))
                        chars_.push_back((Char)i);
                    if(i == max)
                        break;
                }
                random::uniform_int_distribution<std::size_t>::param_type parm(0, chars_.size() - 1);
                dist_.param(parm);
            }

            template<typename Rng>
            Char operator()(Rng &rng)
            {
                return chars_[dist_(rng)];
            }

            friend void set_size_adl(char_class_generator &, std::size_t)
            {}

        private:
            random::uniform_int_distribution<std::size_t> dist_;
            std::vector<Char> chars_;
        };

#define QCHK_DEFINE_CCTYPE_PRED_TYPE(ISCTYPE)                       \
        struct ISCTYPE : std::unary_function<char, bool>            \
        {                                                           \
            bool operator()(char c) const                           \
            {                                                       \
                return 0 != std::ISCTYPE((int)(unsigned char)c);    \
            }                                                       \
        };                                                          \
        /**/

        QCHK_DEFINE_CCTYPE_PRED_TYPE(isalnum)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isalpha)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(iscntrl)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isdigit)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isgraph)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(islower)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isprint)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(ispunct)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isspace)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isupper)
        QCHK_DEFINE_CCTYPE_PRED_TYPE(isxdigit)
    }

    inline detail::char_class_generator<char, detail::isalnum> alnum()
    {
        return detail::char_class_generator<char, detail::isalnum>();
    }

    inline detail::char_class_generator<char, detail::isalpha> alpha()
    {
        return detail::char_class_generator<char, detail::isalpha>();
    }

    inline detail::char_class_generator<char, detail::iscntrl> cntrl()
    {
        return detail::char_class_generator<char, detail::iscntrl>();
    }

    inline detail::char_class_generator<char, detail::isdigit> digit()
    {
        return detail::char_class_generator<char, detail::isdigit>();
    }

    inline detail::char_class_generator<char, detail::isgraph> graph()
    {
        return detail::char_class_generator<char, detail::isgraph>();
    }

    inline detail::char_class_generator<char, detail::islower> lower()
    {
        return detail::char_class_generator<char, detail::islower>();
    }

    inline detail::char_class_generator<char, detail::isprint> print()
    {
        return detail::char_class_generator<char, detail::isprint>();
    }

    inline detail::char_class_generator<char, detail::ispunct> punct()
    {
        return detail::char_class_generator<char, detail::ispunct>();
    }

    inline detail::char_class_generator<char, detail::isspace> space()
    {
        return detail::char_class_generator<char, detail::isspace>();
    }

    inline detail::char_class_generator<char, detail::isupper> upper()
    {
        return detail::char_class_generator<char, detail::isupper>();
    }

    inline detail::char_class_generator<char, detail::isxdigit> xdigit()
    {
        return detail::char_class_generator<char, detail::isxdigit>();
    }

}

QCHK_BOOST_NAMESPACE_END

#endif