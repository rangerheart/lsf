// File:        date.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-04 by leoxiang

#pragma once

#include <ctime>
#include <cstring>
#include "lsf/basic/error.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

class Date : public lsf::basic::Error {
public:
    typedef tm date_type;

    const static time_t SECONDS_PER_DAY = 3600 * 24;
    const static time_t SECONDS_PER_MONTH = 30 * SECONDS_PER_DAY;

public:
    Date(time_t calendar_time = ::time(nullptr)) { _date = *::localtime(&calendar_time); }

    Date(tm const &date) : _date(date) {}

    // must be this format "year-month-day hour:minute:second"
    Date(std::string const &date_string) {
        std::string tmp = date_string;
        util::StringExt::RemoveHeadWhitespace(tmp);
        util::StringExt::RemoveTailWhitespace(tmp);
        size_t year = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 0));
        size_t month = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 1));
        size_t day = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 2));
        size_t hour = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 3));
        size_t minute = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 4));
        size_t second = basic::TypeCast<size_t>(util::StringExt::SplitAndGet(tmp, "- :", 5));
        new (this) Date(year, month, day, hour, minute, second);
    }

    Date(size_t year, size_t month, size_t day, size_t hour, size_t minute, size_t second) {
        Date();
        _date.tm_year = year - 1900;
        _date.tm_mon = month - 1;
        _date.tm_mday = day;
        _date.tm_hour = hour;
        _date.tm_min = minute;
        _date.tm_sec = second;
        _Reset();
    }

    // member funcs
    void SetYear(size_t year) {
        _date.tm_year = year - 1900;
        _Reset();
    }
    void SetMonth(size_t month) {
        _date.tm_mon = month - 1;
        _Reset();
    }
    void SetDay(size_t day) {
        _date.tm_mday = day;
        _Reset();
    }
    void SetHour(size_t hour) {
        _date.tm_hour = hour;
        _Reset();
    }
    void SetMinute(size_t minute) {
        _date.tm_min = minute;
        _Reset();
    }
    void SetSecond(size_t second) {
        _date.tm_sec = second;
        _Reset();
    }

    size_t GetYear() const { return _date.tm_year + 1900; }
    size_t GetMonth() const { return _date.tm_mon + 1; }
    size_t GetDay() const { return _date.tm_mday; }
    size_t GetHour() const { return _date.tm_hour; }
    size_t GetMinute() const { return _date.tm_min; }
    size_t GetSecond() const { return _date.tm_sec; }

    std::string ToFormatString(char const *fmt) const {
        char tmp[512];
        ::strftime(tmp, sizeof(tmp), fmt, &_date);
        return std::string(tmp);
    }

    std::string ToString() { return ToFormatString("%Y-%m-%d %H:%M:%S"); }

    time_t ToCalendarTime() { return ::mktime(&_date); }

    // static members
    static Date Now() { return Date(); }

private:
    void _Reset() {
        // time_t tmp = ::mktime(&_date);
        //_date = *::localtime(&tmp);
    }

    date_type _date;
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
