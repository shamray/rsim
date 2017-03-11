#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>

namespace utils { namespace datetime 
{
  using namespace boost::gregorian;

  inline auto years_ago(double years, date today)
  {
    const auto full_years = boost::numeric_cast<unsigned short>( floor(years) );
    const auto months = boost::numeric_cast<unsigned short>(round((years - full_years) * 11) + 1);

    assert(months >= 1 && months <= 12);

    auto result_year = boost::numeric_cast<unsigned short>(today.year() - full_years);
    auto result_month = boost::numeric_cast<unsigned short>(0);

    if (months > today.month())
    {
      result_month = months + 1 - today.month();
      result_year--;
    }
    else
    {
      result_month = today.month() + 1 - months;
    }

    return date{ result_year, result_month, today.day() };
  }

  inline auto years_after(double years, date today)
  {
    if (years < 0)
      years = 0;

    const auto full_years = boost::numeric_cast<unsigned short>(floor(years));
    const auto months = boost::numeric_cast<unsigned short>(round((years - full_years) * 11) + 1);

    assert(months >= 1 && months <= 12);

    auto result_year = boost::numeric_cast<unsigned short>(today.year() + full_years);
    auto result_month = boost::numeric_cast<unsigned short>(0);

    if (months + today.month() >= 12)
    {
      result_month = abs(months - today.month()) + 1;
      result_year++;
    }
    else
    {
      result_month = today.month() + months;
    }

    return date{ result_year, result_month, today.day() };
  }

  inline auto at_age(int age, date birthday)
  {
    auto year = boost::numeric_cast<unsigned short>(birthday.year() + age);
    return date{ year,  birthday.month(), birthday.day() };
  }
}}