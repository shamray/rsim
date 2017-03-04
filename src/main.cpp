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

  auto operator()()
  {
    return person{};
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
  //for (environment env = {}; *env.current <= date{ 1993, Jan, 1 }; ++env.current)
  for (auto i = 0; i < 1000; ++i)
  {
    cout << env.life_expectancy.male(utils::random::generator()) << endl;
  }
}