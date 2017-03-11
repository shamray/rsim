#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>

namespace sim
{
  enum class gender_t
  {
    male,
    female
  };

  struct person
  {
    using date = boost::gregorian::date;

    date birth_date;
    gender_t gender;

    int salary = 0;
  };
}