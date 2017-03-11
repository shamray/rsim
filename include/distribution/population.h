#pragma once

#include "person.h"

#include "utils/random.h"
#include "utils/datetime.h"

#include <random>

namespace sim
{
  class population_distribution
  {
  public:
    population_distribution(const std::vector<double>& intervals, const std::vector<double>& weights, double male_percentage)
      : age_distribution_(begin(intervals), end(intervals), begin(weights))
      , gender_distribution_(male_percentage)
    {

    }

    auto operator()(boost::gregorian::date today)
    {
      const auto age = age_distribution_(utils::random::generator());
      const auto birth_date = utils::datetime::years_ago(age, today);
      const auto gender = generate_gender();

      return person{ birth_date, gender };
    }

    auto operator()()
    {
      const auto age = age_distribution_(utils::random::generator());
      const auto gender = generate_gender();

      return std::make_tuple(age, gender);
    }

  private:
    auto generate_gender() -> gender_t
    {
      auto is_male = gender_distribution_(utils::random::generator());
      if (is_male)
        return gender_t::male;
      else
        return gender_t::female;
    }

  private:
    std::piecewise_linear_distribution<double> age_distribution_;
    std::bernoulli_distribution gender_distribution_;
  };
}