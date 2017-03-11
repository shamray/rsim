#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>

namespace utils { namespace datetime 
{
  using namespace boost::gregorian;

  auto years_ago(double years, date today) -> date;
  auto years_after(double years, date today) -> date;
  auto at_age(int age, date birthday) -> date;
}}