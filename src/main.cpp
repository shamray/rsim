#include "boost/date_time/gregorian/gregorian.hpp"

#include <random>
#include <iostream>

using namespace boost::gregorian;
using namespace std;

namespace utils { namespace random
{
  auto& generator()
  {
    static std::random_device rd;
    static std::mt19937 gen{ rd() };

    return gen;
  }
}}

namespace utils { namespace datetime 
{
  auto years_ago(double years, date today)
  {
    const auto full_years = boost::numeric_cast<unsigned short>( ceil(years) );
    const auto months = boost::numeric_cast<unsigned short>(round((years - full_years) * 12));

    assert(months >= 1 && months <= 12);

    return date{ full_years, months, today.day() };
  }
}}
enum class gender_t
{
  male,
  female
};

struct environment
{
  month_iterator current = date{ 1991, Sep, 1 };
  double annual_birth_rate = 0;

  struct life_expectancy_t {
    normal_distribution<> male { 61.56, 15 };
    normal_distribution<> female { 74.03, 2.0 };
  };

  life_expectancy_t life_expectancy;

  void event_person_born()
  {}

  void event_person_died()
  {}

  environment(environment&&) = default;
  environment& operator=(environment&&) = default;
};

struct person
{
  date birth_date;
  gender_t gender;
  int life_expectancy;
};

class population_distribution
{
public:
  population_distribution(const vector<double>& intervals, const vector<double>& weights, double male_percentage)
    : age_distribution_(begin(intervals), end(intervals), begin(weights))
    , gender_distribution_(male_percentage)
  {
    
  }

  auto operator()(date today)
  {
    const auto age = age_distribution_(utils::random::generator());
    const auto birth_date = utils::datetime::years_ago(age, today);
    const auto gender = generate_gender();

    return person{ birth_date, gender, -1 };
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
  piecewise_linear_distribution<double> age_distribution_;
  bernoulli_distribution gender_distribution_;
};

auto generate_population(environment& env, const population_distribution& distribution)
{
  constexpr auto size = 1000000;

}

int main()
{
  auto env = environment {};
  //cout << fixed;
  cout.precision(4);
  auto d = population_distribution{ {0, 30, 60, 70, 110}, {2, 2.5, 1.5, 1, 0}, 0.4 };
  //for (environment env = {}; *env.current <= date{ 1993, Jan, 1 }; ++env.current)
  for (auto i = 0; i < 1000; ++i)
  {
    cout << get<0>(d()) << endl;
    //cout << env.life_expectancy.male(utils::random::generator()) << endl;
  }

}