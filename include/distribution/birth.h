#pragma once

#include "person.h"

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

#include <random>

namespace sim
{
  class birth_distribution
  {
    using date = boost::gregorian::date;

  public:
    birth_distribution(double birth_rate_per_woman, double average_delivery_age, std::pair<int, int> fertility_age = { 16, 38 });

    auto operator()(date mother_birth_date, date mother_death_date) -> std::vector<person>;
    auto generate_birth_dates(date mother_birth_date, date mother_death_date) -> std::vector<date>;

  private:
    auto generate_ages(int number_of_children)->std::vector<double>;
    auto to_dates(std::vector<double> age_of_delivery, date mother_birth_date) -> std::vector<date>;
    auto satisfies(std::vector<date> dates_of_delivery, date mother_death_date) -> bool;
    auto satisfies(std::vector<double> age_of_delivery) -> bool;
    auto generate_gender()->gender_t;

  private:
    std::poisson_distribution<> number_of_children_;
    std::normal_distribution<> age_of_delivery_;
    std::bernoulli_distribution gender_distribution_;
    int min_age_;
    int max_age_;
  };
}