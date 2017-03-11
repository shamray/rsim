#pragma once

#include "person.h"
#include <random>

namespace sim
{
  class life_expectancy_distribution
  {
  public:
    life_expectancy_distribution(double male_mean, double female_mean)
      : male_{ male_mean, 12 }
      , female_{ female_mean, 12 }
    {}

    auto operator()(gender_t gender)
    {
      if (gender == gender_t::male)
        return male_(utils::random::generator());
      else
        return female_(utils::random::generator());
    }

    auto operator()(boost::gregorian::date today, gender_t gender)
    {
      return utils::datetime::years_after(operator()(gender), today);
    }

  private:
    std::normal_distribution<> male_;
    std::normal_distribution<> female_;
  };
}