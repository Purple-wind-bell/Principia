
#pragma once

#include "astronomy/date.hpp"

#include <array>
#include <cstdint>

#include "astronomy/epoch.hpp"
#include "geometry/named_quantities.hpp"
#include "glog/logging.h"
#include "quantities/si.hpp"

// A macro to allow glog checking within C++11 constexpr code.  If |condition|,
// evaluates to |expression|.  Otherwise, results in a CHECK failure at runtime
// and a compilation error due to a call to non-constexpr code at compile time.
#define CHECKING(condition, expression) \
  ((condition) ? (expression) : (CHECK(condition), (expression)))

namespace principia {
namespace astronomy {
namespace internal_date {

using quantities::Time;
using quantities::si::Day;
using quantities::si::Second;

// The following is horrendous in part because written in C++11, thus
// functionally; it should perhaps be reworked once MSVC offers proper C++14
// constexpr.

// Calendar classes.

class Date {
 public:
  static constexpr Date YYYYMMDD(std::int64_t digits);
  static constexpr Date YYYYwwD(std::int64_t digits);
  static constexpr Date YYYYDDD(std::int64_t digits);

  static constexpr Date Calendar(int const year,
                                 int const month,
                                 int const day);
  static constexpr Date Ordinal(int const year, int const day);
  static constexpr Date Week(int const year, int const week, int const day);

  constexpr int year() const;
  constexpr int month() const;
  constexpr int day() const;

  constexpr int ordinal() const;

  constexpr Date next_day() const;

 private:
  constexpr Date(int const year,
                 std::int8_t const month,
                 std::int8_t const day);

  int const year_;
  int const month_;
  int const day_;
};

class TimeOfDay {
 public:
  static constexpr TimeOfDay hhmmss_ns(int const hhmmss, int ns);

  constexpr int hour() const;
  constexpr int minute() const;
  constexpr int second() const;
  constexpr int nanosecond() const;

  constexpr bool is_leap_second() const;
  // Whether |*this| is 24:00:00.
  constexpr bool is_end_of_day() const;

 private:
  constexpr TimeOfDay(int const hour,
                      int const minute,
                      int const second,
                      int const nanosecond);

  // Checks that this represents a valid time of day as per ISO 8601, thus
  // that the components are in the normal range, or that the object represents
  // a time in a leap second, or that it represents the end of the day.
  constexpr TimeOfDay const& checked() const;

  int const hour_;
  int const minute_;
  int const second_;
  int const nanosecond_;
};

class DateTime {
 public:
  constexpr Date const& date() const;
  constexpr TimeOfDay const& time() const;

  // If |time()| is 24:00:00, returns an equivalent DateTime where midnight is
  // expressed as 00:00:00 on the next day; otherwise, returns *this.
  constexpr DateTime normalized_end_of_day() const;

 private:
  constexpr DateTime(Date const date, TimeOfDay const time);

  // Checks that |time| does not represent a leap second unless |date| is the
  // last day of June, December, March, or September.
  constexpr DateTime const& checked() const;

  Date const date_;
  TimeOfDay const time_;

  friend constexpr DateTime operator""_DateTime(char const* string,
                                                 std::size_t size);
};

// Arithmetico-calendrical utility functions.

constexpr std::array<int, 12> non_leap_year_month_lengths{
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

constexpr bool is_gregorian_leap_year(int const year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

constexpr int gregorian_year_length(int const year) {
  return is_gregorian_leap_year(year) ? 366 : 365;
}

constexpr int month_length(int year, int month) {
  return (is_gregorian_leap_year(year) && month == 2)
             ? 29
             : non_leap_year_month_lengths[month - 1];
}

constexpr int mod_7_offset_1 (int const x) {
  return x % 7 == 0 ? 7 : (x % 7);
}

// Result in [1, 7], 1 is Monday.
constexpr int day_of_week_on_january_1st(int const year) {
  // Gauss's formula, see
  // https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Gauss.27s_algorithm.
  return mod_7_offset_1(1 +
                        5 * ((year - 1) % 4) +
                        4 * ((year - 1) % 100) +
                        6 * ((year - 1) % 400));
}

constexpr bool is_53_week_year(int const year) {
  return day_of_week_on_january_1st(year) == 4 ||
         (is_gregorian_leap_year(year) &&
          day_of_week_on_january_1st(year) == 3);
}

constexpr int ordinal_of_w_01_1(int const year) {
  return day_of_week_on_january_1st(year) <= 4
             ? 2 - day_of_week_on_january_1st(year)
             : (9 - day_of_week_on_january_1st(year));
}

constexpr int days_from_2000_01_01_at_start_of_2000_based_year(
    int const years_from_2000) {
  return years_from_2000 > 0
             ? 1 + years_from_2000 * 365 +
               (years_from_2000 - 1) / 4 -
               (years_from_2000 - 1) / 100 +
               (years_from_2000 - 1) / 400
             : years_from_2000 * 365 +
               years_from_2000 / 4 -
               years_from_2000 / 100 +
               years_from_2000 / 400;
}

// Returns the number formed by taking |size| increasingly significant digits,
// starting from the digit of the (10 ** |begin|)s.
constexpr std::int64_t digit_range(std::int64_t const digits,
                                   int const begin,
                                   int const size) {
  return CHECKING(
      digits >= 0 && begin >= 0 && size >= 0,
      (size == 0) ? 0 : ((begin == 0)
                             ? digit_range(digits / 10, 0, size - 1) * 10 +
                                   digits % 10
                             : digit_range(digits / 10, begin - 1, size)));
}

// Returns x * 10 ** count.
constexpr std::int64_t add_0s(std::int64_t const x, int const count) {
  return CHECKING(count >= 0, count == 0 ? x : add_0s(x * 10, count - 1));
}

// Returns |date| advanced by the specified number of |days|. |days| must be
// nonnegative, and the result must be in the same year.
constexpr Date add_days_within_year(Date const& date, int const days) {
  return CHECKING(
      days >= 0,
      days == 0
          ? date
          : (date.day() + days > month_length(date.year(), date.month())
                 ? CHECKING(
                       date.month() <= 11,
                       add_days_within_year(
                           Date::Calendar(date.year(), date.month() + 1, 1),
                           days - month_length(date.year(), date.month()) +
                               date.day() - 1))
                 : Date::Calendar(date.year(),
                                  date.month(),
                                  date.day() + days)));
}

// The |day|th day of some |year|.  The resulting date need not be in |year|.
constexpr Date ArbitraryOrdinal(int const year, int const day) {
  return day < 1
             ? ArbitraryOrdinal(year - 1, gregorian_year_length(year - 1) + day)
             : day > gregorian_year_length(year)
                   ? ArbitraryOrdinal(year + 1,
                                      day - gregorian_year_length(year))
                   : Date::Ordinal(year, day);
}

// Implementation of class |Date|.

constexpr Date Date::YYYYMMDD(std::int64_t digits) {
  return CHECKING(digits > 0 && digits < 9999'99'99,
                  Date::Calendar(digit_range(digits, 4, 4),
                                 digit_range(digits, 2, 2),
                                 digit_range(digits, 0, 2)));
}

constexpr Date Date::YYYYDDD(std::int64_t digits) {
  return CHECKING(
      digits > 0 && digits < 9999'999,
      Date::Ordinal(digit_range(digits, 3, 4), digit_range(digits, 0, 3)));
}

constexpr Date Date::YYYYwwD(std::int64_t digits) {
  return CHECKING(digits > 0 && digits < 9999'99'9,
                  Date::Week(digit_range(digits, 3, 4),
                             digit_range(digits, 1, 2),
                             digit_range(digits, 0, 1)));
}

constexpr Date Date::Calendar(int const year, int const month, int const day) {
  return CHECKING(year >= 1583 && year <= 9999 && month >= 1 && month <= 12 &&
                  day >= 1 && day <= month_length(year, month),
                  Date(year, month, day));
}

constexpr Date Date::Ordinal(int const year, int const day) {
  return CHECKING(
      day >= 1 && day <= gregorian_year_length(year),
      add_days_within_year(Date::Calendar(year, 1, 1), day - 1));
}

constexpr Date Date::Week(int const year, int const week, int const day) {
  return CHECKING(week >= 1 && week <= (is_53_week_year(year) ? 53 : 52) &&
                  day >= 1 && day <= 7,
                  ArbitraryOrdinal(
                      year,
                      (week - 1) * 7 + day - 1 + ordinal_of_w_01_1(year)));
}

constexpr int Date::year() const {
  return year_;
}

constexpr int Date::month() const {
  return month_;
}

constexpr int Date::day() const {
  return day_;
}

constexpr int Date::ordinal() const {
  return day_ > 1 ? (day_ - 1) + Date(year_, month_, 1).ordinal()
                  : month_ > 1
                        ? month_length(year_, month_ - 1) +
                              Date(year_, month_ - 1, 1).ordinal()
                        : 1;
}

constexpr Date Date::next_day() const {
  return day_ == month_length(year_, month_)
             ? month_ == 12 ? Date(year_ + 1, 1, 1) : Date(year_, month_ + 1, 1)
             : Date(year_, month_, day_ + 1);
}

constexpr Date::Date(int const year,
                     std::int8_t const month,
                     std::int8_t const day)
      : year_(year),
        month_(month),
        day_(day) {}

// Implementation of class TimeOfDay.

constexpr TimeOfDay TimeOfDay::hhmmss_ns(int const hhmmss, int ns) {
  return TimeOfDay(digit_range(hhmmss, 4, 2),
                   digit_range(hhmmss, 2, 2),
                   digit_range(hhmmss, 0, 2),
                   ns).checked();
}

constexpr int TimeOfDay::hour() const {
  return hour_;
}

constexpr int TimeOfDay::minute() const {
  return minute_;
}

constexpr int TimeOfDay::second() const {
  return second_;
}

constexpr int TimeOfDay::nanosecond() const {
  return nanosecond_;
}

constexpr bool TimeOfDay::is_leap_second() const {
  return second_ == 60;
}

constexpr bool TimeOfDay::is_end_of_day() const {
  return hour_ == 24;
}

constexpr TimeOfDay::TimeOfDay(int const hour,
                               int const minute,
                               int const second,
                               int const nanosecond)
    : hour_(hour),
      minute_(minute),
      second_(second),
      nanosecond_(nanosecond) {}

constexpr TimeOfDay const& TimeOfDay::checked() const {
  return CHECKING(
      (hour_ == 24 && minute_ == 0 && second_ == 0 && nanosecond_ == 0) ||
          ((nanosecond_ >= 0 && nanosecond_ <= 999'999'999) &&
           ((hour_ == 23 && minute_ == 59 && second_ == 60) ||
            (hour_ >= 0 && hour_ <= 23 && minute_ >= 0 && minute_ <= 59 &&
             second_ >= 0 && second_ <= 59))),
      *this);
}

// Implementation of class DateTime.

constexpr Date const& DateTime::date() const {
  return date_;
}

constexpr TimeOfDay const& DateTime::time() const {
  return time_;
}

constexpr DateTime DateTime::normalized_end_of_day() const {
  return time_.is_end_of_day()
             ? DateTime(date_.next_day(), TimeOfDay::hhmmss_ns(00'00'00, 0))
             : *this;
}

constexpr DateTime::DateTime(Date const date, TimeOfDay const time)
    : date_(date),
      time_(time) {}

constexpr DateTime const& DateTime::checked() const {
  return CHECKING(
      !time_.is_leap_second() ||
          (date_.day() == month_length(date_.year(), date_.month()) &&
           (date_.month() == 6 || date_.month() == 12 || date_.month() == 3 ||
            date_.month() == 9)),
      *this);
}

// Date parsing.

struct DateStringInfo {
  constexpr DateStringInfo Fill() const {
    return read == size
               ? *this
               : string[read] == '-'
                     ? CHECKING(
                           hyphens < 2,
                           hyphens == 0
                               ? DateStringInfo({string,
                                                 size,
                                                 read + 1,
                                                 digits,
                                                 digit_count,
                                                 hyphens + 1,
                                                 /*first_hyphen_index=*/read,
                                                 second_hyphen_index,
                                                 has_w,
                                                 w_index}).Fill()
                               : DateStringInfo({string,
                                                 size,
                                                 read + 1,
                                                 digits,
                                                 digit_count,
                                                 hyphens + 1,
                                                 first_hyphen_index,
                                                 /*second_hyphen_index=*/read,
                                                 has_w,
                                                 w_index})).Fill()
                     : string[read] == 'W'
                           ? CHECKING(!has_w,
                                      DateStringInfo({string,
                                                      size,
                                                      read + 1,
                                                      digits,
                                                      digit_count,
                                                      hyphens,
                                                      first_hyphen_index,
                                                      second_hyphen_index,
                                                      /*has_w=*/true,
                                                      /*w_index=*/read})).Fill()
                           : CHECKING(
                                 string[read] >= '0' && string[read] <= '9',
                                 DateStringInfo(
                                     {string,
                                      size,
                                      read + 1,
                                      digits * 10 + string[read] - '0',
                                      digit_count + 1,
                                      hyphens,
                                      first_hyphen_index,
                                      second_hyphen_index,
                                      has_w,
                                      w_index}).Fill());
  }

  constexpr Date ToDate() const {
    return digit_count == 8
               ? CHECKING(
                     hyphens == 0 || (hyphens == 2 && first_hyphen_index == 4 &&
                                      second_hyphen_index == 7),
                     Date::YYYYMMDD(digits))
               : CHECKING(
                     digit_count == 7,
                     has_w ? CHECKING(
                                 (hyphens == 0 && w_index == 4) ||
                                     (hyphens == 2 && first_hyphen_index == 4 &&
                                      w_index == 5 && second_hyphen_index == 8),
                                 Date::YYYYwwD(digits))
                           : CHECKING(hyphens == 0 || (hyphens == 1 &&
                                                       first_hyphen_index == 4),
                                      Date::YYYYDDD(digits)));
  }

  char const* const string;
  std::size_t size;
  int const read;
  std::int64_t digits;
  int digit_count;
  int const hyphens;
  int const first_hyphen_index;
  int const second_hyphen_index;
  bool const has_w;
  int const w_index;
};

constexpr Date operator""_Date(char const* string, std::size_t size) {
  return DateStringInfo{string,
                        size,
                        /*read=*/0,
                        /*digits=*/0,
                        /*digit_count=*/0,
                        /*hyphens=*/0,
                        /*first_hyphen_index=*/-1,
                        /*second_hyphen_index=*/-1,
                        /*has_w=*/false,
                        /*w_index=*/-1}.Fill().ToDate();
}

// Time parsing.

struct TimeStringInfo {
  constexpr TimeStringInfo Fill() const {
    return read == size
               ? *this
               : string[read] == ':'
                     ? CHECKING(
                           colons < 2,
                           colons == 0
                               ? TimeStringInfo({string,
                                                 size,
                                                 read + 1,
                                                 digits,
                                                 digit_count,
                                                 colons + 1,
                                                 /*first_colon_index=*/read,
                                                 second_colon_index,
                                                 has_decimal_mark,
                                                 decimal_mark_index}).Fill()
                               : TimeStringInfo({string,
                                                 size,
                                                 read + 1,
                                                 digits,
                                                 digit_count,
                                                 colons + 1,
                                                 first_colon_index,
                                                 /*second_colon_index=*/read,
                                                 has_decimal_mark,
                                                 decimal_mark_index})).Fill()
                     : string[read] == ',' || string[read] == '.'
                           ? CHECKING(
                                 !has_decimal_mark,
                                 TimeStringInfo(
                                    {string,
                                     size,
                                     read + 1,
                                     digits,
                                     digit_count,
                                     colons,
                                     first_colon_index,
                                     second_colon_index,
                                     /*has_decimal_mark=*/true,
                                     /*decimal_mark_index=*/read}).Fill())
                           : string[read] == 'Z'
                                 ? CHECKING(
                                       read == size - 1,
                                       TimeStringInfo(
                                           {string,
                                            size,
                                            read + 1,
                                            digits,
                                            digit_count,
                                            colons,
                                            first_colon_index,
                                            second_colon_index,
                                            has_decimal_mark,
                                            decimal_mark_index}).Fill())
                                 : CHECKING(
                                       string[read] >= '0' &&
                                       string[read] <= '9',
                                       TimeStringInfo(
                                           {string,
                                            size,
                                            read + 1,
                                            digits * 10 + string[read] - '0',
                                            digit_count + 1,
                                            colons,
                                            first_colon_index,
                                            second_colon_index,
                                            has_decimal_mark,
                                            decimal_mark_index}).Fill());
  }

  constexpr TimeOfDay ToTime() const {
    return CHECKING(
        digit_count >= 6 &&
            (colons == 0 || (colons == 2 && first_colon_index == 2 &&
                             second_colon_index == 5)) &&
            ((digit_count == 6 && !has_decimal_mark) ||
             (has_decimal_mark &&
              ((colons == 0 && decimal_mark_index == 6) ||
               (colons != 0 && decimal_mark_index == 8)))) &&
            digit_count <= 15 &&
            string[size - 1] == 'Z',
        TimeOfDay::hhmmss_ns(digit_range(digits, digit_count - 6, 6),
                           add_0s(digit_range(digits, 0, digit_count - 6),
                                  9 - (digit_count - 6))));
  }

  char const* const string;
  std::size_t size;
  int const read;
  std::int64_t digits;
  int digit_count;
  int const colons;
  int const first_colon_index;
  int const second_colon_index;
  bool const has_decimal_mark;
  int const decimal_mark_index;
};

constexpr TimeOfDay operator""_Time(char const* string, std::size_t size) {
  return TimeStringInfo{string,
                        size,
                        /*read=*/0,
                        /*digits=*/0,
                        /*digit_count=*/0,
                        /*colons=*/0,
                        /*first_colon_index=*/-1,
                        /*second_colon_index=*/-1,
                        /*has_decimal_mark=*/false,
                        /*decimal_mark_index=*/-1}.Fill().ToTime();
}

// DateTime parsing.

constexpr bool contains(char const* string, std::size_t size, char const c) {
  return size > 0 && (string[0] == c || contains(string + 1, size - 1, c));
}

constexpr int index_of(char const* string, std::size_t size, char const c) {
  return CHECKING(size > 0,
                  string[0] == c ? 0 : (index_of(string + 1, size - 1, c) + 1));
}

constexpr DateTime operator""_DateTime(char const* string, std::size_t size) {
  return CHECKING(
      contains(string, size, '-') == contains(string, size, ':'),
      DateTime(
          operator""_Date(string, index_of(string, size, 'T')),
          operator""_Time(string + index_of(string, size, 'T') + 1,
                          size - (index_of(string, size, 'T') + 1))).checked());
}

// Conversion to |Instant|, continuous time scales.

constexpr Instant DateTimeAsTT(DateTime const& date_time) {
  return CHECKING(!date_time.time().is_leap_second(),
                  J2000 +
                  date_time.time().nanosecond() / 1e9 * Second +
                  (date_time.time().second() +
                   date_time.time().minute() * 60 +
                   (date_time.time().hour() - 12) * 60 * 60) * Second +
                  (days_from_2000_01_01_at_start_of_2000_based_year(
                       date_time.date().year() - 2000) +
                   date_time.date().ordinal() - 1) * Day);
}

constexpr Instant DateTimeAsTAI(DateTime const& date_time) {
  return CHECKING(!date_time.time().is_leap_second(),
                  J2000 +
                  (date_time.time().nanosecond() + 184'000'000) / 1e9 * Second +
                  ((date_time.time().second() - 28) +
                   (date_time.time().minute() - 59) * 60 +
                   (date_time.time().hour() - 11) * 60 * 60) * Second +
                  (days_from_2000_01_01_at_start_of_2000_based_year(
                       date_time.date().year() - 2000) +
                   date_time.date().ordinal() - 1) * Day);
}

// Leap second handling and conversion to UTC.

constexpr std::array<int, (2016 - 1972) * 2 + 1> leap_seconds = {{
    +1, +1,  // 1972
    +0, +1,  // 1973
    +0, +1,  // 1974
    +0, +1,  // 1975
    +0, +1,  // 1976
    +0, +1,  // 1977
    +0, +1,  // 1978
    +0, +1,  // 1979
    +0, +0,  // 1980
    +1, +0,  // 1981
    +1, +0,  // 1982
    +1, +0,  // 1983
    +0, +0,  // 1984
    +1, +0,  // 1985
    +0, +0,  // 1986
    +0, +1,  // 1987
    +0, +0,  // 1988
    +0, +1,  // 1989
    +0, +1,  // 1990
    +0, +0,  // 1991
    +1, +0,  // 1992
    +1, +0,  // 1993
    +1, +0,  // 1994
    +0, +1,  // 1995
    +0, +0,  // 1996
    +1, +0,  // 1997
    +0, +1,  // 1998
    +0, +0,  // 1999
    +0, +0,  // 2000
    +0, +0,  // 2001
    +0, +0,  // 2002
    +0, +0,  // 2003
    +0, +0,  // 2004
    +0, +1,  // 2005
    +0, +0,  // 2006
    +0, +0,  // 2007
    +0, +1,  // 2008
    +0, +0,  // 2009
    +0, +0,  // 2010
    +0, +0,  // 2011
    +1, +0,  // 2012
    +0, +0,  // 2013
    +0, +0,  // 2014
    +1, +0,  // 2015
    +0,      // 2016
}};

// Returns UTC - TAI on the given UTC day (similar to Bulletin C).
constexpr Time UTC_TAI(Date const& utc_date) {
  return utc_date.month() == 1 && utc_date.day() == 1
             ? utc_date.year() == 1972
                   ? -10 * Second
                   : -leap_seconds[(utc_date.year() - 1973) * 2] * Second +
                     -leap_seconds[(utc_date.year() - 1973) * 2 + 1] * Second +
                     UTC_TAI(Date::Calendar(utc_date.year() - 1, 1, 1))
             : (utc_date.month() > 6
                    ? -leap_seconds[(utc_date.year() - 1972) * 2] * Second
                    : 0 * Second) +
               UTC_TAI(Date::Calendar(utc_date.year(), 1, 1));
}

// NOTE(egg): no check for invalid UTC in case of negative leap seconds.
constexpr Instant DateTimeAsUTC(DateTime const& date_time) {
  return date_time.time().is_end_of_day()
             ? DateTimeAsUTC(date_time.normalized_end_of_day())
             : CHECKING(
                   !date_time.time().is_leap_second() ||
                   (date_time.date().month() == 6 &&
                    leap_seconds[(date_time.date().year() - 1972) * 2] == +1) ||
                   (date_time.date().month() == 12 &&
                    leap_seconds[(date_time.date().year() - 1972) * 2 + 1] ==
                        +1),
                   DateTimeAsTAI(date_time) - UTC_TAI(date_time.date()));
}

// |Instant| date literals.

#if PRINCIPIA_COMPILER_CLANG || PRINCIPIA_COMPILER_CLANG_CL
template<typename C, C... string>
constexpr std::array<C, sizeof...(string)> unpack_as_array() {
  return std::array<C, sizeof...(string)>{{string...}};
}

template<typename T>
constexpr T const& as_const_ref(T const& t) {
  return t;
}

template<typename C, std::size_t size>
constexpr C const* c_str(std::array<C, size> array) {
  // In C++17 this could be |data()|.  For the moment this does the job.
  return &as_const_ref(array)[0];
}

template<typename C, C... string>
constexpr Instant operator""_TAI() {
  return DateTimeAsTAI(
      operator""_DateTime(c_str(unpack_as_array<C, string...>()),
                          sizeof...(string)));
}

template<typename C, C... string>
constexpr Instant operator""_TT() {
  return DateTimeAsTT(
      operator""_DateTime(c_str(unpack_as_array<C, string...>()),
                          sizeof...(string)));
}

template<typename C, C... string>
constexpr Instant operator""_UTC() {
  return DateTimeAsUTC(
      operator""_DateTime(c_str(unpack_as_array<C, string...>()),
                          sizeof...(string)));
}
#else
constexpr Instant operator""_TAI(char const* string, std::size_t size) {
  return DateTimeAsTAI(operator""_DateTime(string, size));
}

constexpr Instant operator""_TT(char const* string, std::size_t size) {
  return DateTimeAsTT(operator""_DateTime(string, size));
}

constexpr Instant operator""_UTC(char const* string, std::size_t size) {
  return DateTimeAsUTC(operator""_DateTime(string, size));
}
#endif

}  // namespace internal_date
}  // namespace astronomy
}  // namespace principia
