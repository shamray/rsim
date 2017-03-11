#include "utils/random.h"
#include "utils/datetime.h"

#include "distribution/population.h"
#include "distribution/life_expectancy.h"
#include "distribution/salary.h"
#include "distribution/birth.h"

#include "person.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/numeric.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>

#include <random>
#include <unordered_map>
#include <iostream>

using namespace boost::accumulators;
using namespace boost::gregorian;
using namespace std;

using namespace sim;

struct environment
{
  month_iterator current = date{ 1991, Sep, 1 };
  birth_distribution childbirth{ 2.1, 26 };
  long id = 0;
  unordered_map<long, person> population;
  multimap<date, function<void()>> events;

  environment(environment&&) = default;
  environment& operator=(environment&&) = default;
};

auto retirement_age(const person& p)
{
  if (p.gender == gender_t::male)
    return 60;
  else
    return 55;
}

void add_person(environment& env, person new_person, life_expectancy_distribution& led, salary_distribution& sd)
{
  auto id = ++env.id;
  env.population[id] = new_person;

  auto death_date = led(*env.current, new_person.gender);
  env.events.insert(make_pair(death_date, [&env, id]() { env.population.erase(id); }));

  auto salary = sd();
  auto work_start = utils::datetime::at_age(18, new_person.birth_date);
  env.events.insert(make_pair(work_start, [&env, &p = env.population[id], salary]() { p.salary = salary; }));

  auto retirement = utils::datetime::at_age(retirement_age(new_person), new_person.birth_date);
  env.events.insert(make_pair(retirement, [&env, &p = env.population[id]]() { p.salary = 0; }));

  if (new_person.gender == gender_t::female)
  {
    auto children = env.childbirth(new_person.birth_date, death_date);
    for (auto&& child : children)
    {
      if (child.birth_date < *env.current)
        continue;

      env.events.insert(make_pair(child.birth_date, [&env, &led, &sd, child]() { 
        add_person(env, child, led, sd); 
      }));
    }
  }
}

auto generate_population(environment& env, population_distribution& distribution, int size, life_expectancy_distribution& led, salary_distribution& sd)
{
  for (auto i = 0; i < size; ++i)
    add_person(env, distribution(*env.current), led, sd);
}

int main()
{
  cout.precision(4);

  auto env = environment {};
  auto pd = population_distribution{ {0, 30, 60, 70, 110}, {2, 2.5, 1.5, 1, 0}, 0.4 };
  auto led = life_expectancy_distribution{ 61.56, 74.03 };
  auto sd = salary_distribution{ 1000, 400 };

  cout << "generating population..." << endl;

  generate_population(env, pd, 10000, led, sd);

  for (; *env.current <= date{ 2175, Jan, 1 }; ++env.current)
  {
    for (auto&& event_pair : boost::make_iterator_range(env.events.begin(), env.events.upper_bound(*env.current)))
    {
      event_pair.second();
    }

    env.events.erase(env.events.begin(), env.events.upper_bound(*env.current));

    if (env.current->month() == Jan)
      cout << *env.current << "  " << env.population.size() << " \t" << boost::accumulate(env.population, 0, [](int s, auto&& pp) { return s + pp.second.salary; }) << endl;
  }

  cout << "finished" << endl;
}
