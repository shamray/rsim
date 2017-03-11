#include "distribution/birth.h"
#include "utils/random.h"
#include "utils/datetime.h"

namespace sim
{
  birth_distribution::birth_distribution(double birth_rate_per_woman, double average_delivery_age, std::pair<int, int> fertility_age)
    : number_of_children_(birth_rate_per_woman)
    , age_of_delivery_(average_delivery_age, 5)
    , gender_distribution_(0.5)
    , min_age_(fertility_age.first)
    , max_age_(fertility_age.second)
  {}

  auto birth_distribution::operator()(date mother_birth_date, date mother_death_date) -> std::vector<person>
  {
    auto result = std::vector<person>{};
    boost::copy(
      generate_birth_dates(mother_birth_date, mother_death_date)
      | boost::adaptors::transformed([&](auto &&birthday) {return person{ birthday, generate_gender() }; }),
      back_inserter(result)
    );
    return result;
  }

  auto birth_distribution::generate_birth_dates(date mother_birth_date, date mother_death_date) -> std::vector<date>
  {
    auto number_of_children = number_of_children_(utils::random::generator());
    for (;;)
    {
      auto age_of_delivery = generate_ages(number_of_children);
      if (!satisfies(age_of_delivery))
        continue;

      auto dates_of_delivery = to_dates(age_of_delivery, mother_birth_date);
      if (!satisfies(dates_of_delivery, mother_birth_date))
        continue;

      return dates_of_delivery;
    }
  }

  auto birth_distribution::generate_ages(int number_of_children) ->std::vector<double>
  {
    auto result = std::vector<double>{};
    for (auto i = 0; i < number_of_children; ++i)
      result.push_back(age_of_delivery_(utils::random::generator()));

    return result;
  }

  auto birth_distribution::to_dates(std::vector<double> age_of_delivery, date mother_birth_date) -> std::vector<date>
  {
    auto result = std::vector<date>{};
    boost::copy(
      age_of_delivery
      | boost::adaptors::transformed([mother_birth_date](auto &&age) {return utils::datetime::years_after(age, mother_birth_date); }),
      back_inserter(result)
    );
    return result;
  }

  auto birth_distribution::satisfies(std::vector<date> dates_of_delivery, date mother_death_date) -> bool
  {
    if (dates_of_delivery.empty())
      return true;

    return *boost::max_element(dates_of_delivery) >= mother_death_date;
  }

  auto birth_distribution::satisfies(std::vector<double> age_of_delivery) -> bool
  {
    if (age_of_delivery.empty())
      return true;

    auto min = boost::min_element(age_of_delivery);
    auto max = boost::max_element(age_of_delivery);

    return *min >= min_age_ && *max <= max_age_;
  }

  auto birth_distribution::generate_gender() -> gender_t
  {
    auto is_male = gender_distribution_(utils::random::generator());
    if (is_male)
      return gender_t::male;
    else
      return gender_t::female;
  }
}