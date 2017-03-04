#include "boost/date_time/gregorian/gregorian.hpp"

#include <random>
#include <iostream>

using namespace boost::gregorian;
using namespace std;

struct person
{
  date birth_date;
  int life_expectancy_t;
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

  std::random_device rd;
  std::mt19937 gen{ rd() };

  environment(environment&&) = default;
  environment& operator=(environment&&) = default;
};

struct population_distribution
{
  vector<int> intervals;
  vector<int> weights;
};
auto generate_population(environment& env, const population_distribution& distribution)
{
  constexpr auto size = 1000000;

}

int main()
{
  environment env {};
  //cout << fixed;
  cout.precision(4);
  //for (environment env = {}; *env.current <= date{ 1993, Jan, 1 }; ++env.current)
  for (auto i = 0; i < 1000; ++i)
  {
    cout << env.life_expectancy.male(env.gen) << endl;
  }
}