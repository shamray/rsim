#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
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
    const auto full_years = boost::numeric_cast<unsigned short>( floor(years) );
    const auto months = boost::numeric_cast<unsigned short>(round((years - full_years) * 11) + 1);

    assert(months >= 1 && months <= 12);

    auto result_year = boost::numeric_cast<unsigned short>(today.year() - full_years);
    auto result_month = boost::numeric_cast<unsigned short>(0);

    if (months > today.month())
    {
      result_month = months + 1 - today.month();
      result_year--;
    }
    else
    {
      result_month = today.month() + 1 - months;
    }

    return date{ result_year, result_month, today.day() };
  }

  auto years_after(double years, date today)
  {
    const auto full_years = boost::numeric_cast<unsigned short>(floor(years));
    const auto months = boost::numeric_cast<unsigned short>(round((years - full_years) * 11) + 1);

    assert(months >= 1 && months <= 12);

    auto result_year = boost::numeric_cast<unsigned short>(today.year() + full_years);
    auto result_month = boost::numeric_cast<unsigned short>(0);

    if (months + today.month() >= 12)
    {
      result_month = abs(months - today.month()) + 1;
      result_year++;
    }
    else
    {
      result_month = today.month() + months;
    }

    return date{ result_year, result_month, today.day() };
  }

  auto at_age(int age, date birthday)
  {
    auto year = boost::numeric_cast<unsigned short>(birthday.year() + age);
    return date{ year,  birthday.month(), birthday.day() };
  }
}}

enum class gender_t
{
  male,
  female
};

struct person
{
  date birth_date;
  gender_t gender;

  int salary = 0;
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
  piecewise_linear_distribution<double> age_distribution_;
  bernoulli_distribution gender_distribution_;
};

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

  auto operator()(date today, gender_t gender)
  {
    return utils::datetime::years_after(operator()(gender), today);
  }

private:
  normal_distribution<> male_;
  normal_distribution<> female_;
};

class salary_distribution
{
public:
  salary_distribution(int mean, int sigma)
    : dist_{ m(mean, sigma * sigma), s(mean, sigma * sigma) }
  {}

  auto operator()()
  {
    return boost::numeric_cast<long>( round(dist_(utils::random::generator())) );
  }

private:
  auto m(double mean, double variance) -> double
  {
    return log(mean * mean / sqrt(variance + mean * mean));
  }

  auto s(double mean, double variance) -> double
  {
    return sqrt(log( 1 + variance / (mean * mean)));
  }

private:
  lognormal_distribution<> dist_;
};

class birth_distribution
{
public:
  birth_distribution(double birth_rate_per_woman, double average_delivery_age, std::pair<int, int> fertility_age = { 16, 38 })
    : number_of_children_(birth_rate_per_woman)
    , age_of_delivery_(average_delivery_age, 5)
    , gender_distribution_(0.5)
    , min_age_(fertility_age.first)
    , max_age_(fertility_age.second)
  {}

  auto operator()(date mother_birth_date, date mother_death_date)
  {
    auto result = vector<person>{};
    boost::copy(
      generate_birth_dates(mother_birth_date, mother_death_date)
      | boost::adaptors::transformed([&](auto &&birthday) {return person{ birthday, generate_gender() }; }),
      back_inserter(result)
    );
    return result;
  }

  auto generate_birth_dates(date mother_birth_date, date mother_death_date) -> vector<date>
  {
    if (mother_death_date < utils::datetime::years_after(min_age_, mother_birth_date))
      return {};

    auto number_of_children = number_of_children_(utils::random::generator());
    auto last_delivery = utils::datetime::years_after(max_age_, mother_birth_date);

    if (number_of_children >= last_delivery.year() - mother_birth_date.year())
      number_of_children = (last_delivery.year() - mother_birth_date.year()) / 2;

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

private:
  auto generate_ages(int number_of_children) ->vector<double>
  {
    auto result = vector<double>{};
    for (auto i = 0; i < number_of_children; ++i)
      result.push_back(age_of_delivery_(utils::random::generator()));
    
    return result;
  }

  auto to_dates(vector<double> age_of_delivery, date mother_birth_date) -> vector<date>
  {
    auto result = vector<date>{};
    boost::copy(
      age_of_delivery
      | boost::adaptors::transformed([mother_birth_date](auto &&age) {return utils::datetime::years_after(age, mother_birth_date); }),
      back_inserter(result)
    );
    return result;
  }

  auto satisfies(vector<date> dates_of_delivery, date mother_death_date) -> bool
  {
    if (dates_of_delivery.empty())
      return true;

    return *boost::max_element(dates_of_delivery) >= mother_death_date;
  }

  auto satisfies(vector<double> age_of_delivery) -> bool
  {
    auto distance_less_than_year = boost::adjacent_find(
      age_of_delivery,
      [](auto&& x, auto &&y) { return x - y < 1; }
    );
    if (distance_less_than_year != age_of_delivery.end())
      return false;

    if (age_of_delivery.empty())
      return true;

    auto min = boost::min_element(age_of_delivery);
    auto max = boost::max_element(age_of_delivery);

    return *min >= min_age_ && *max <= max_age_;
  }

  auto generate_gender() -> gender_t
  {
    auto is_male = gender_distribution_(utils::random::generator());
    if (is_male)
      return gender_t::male;
    else
      return gender_t::female;
  }

private:
  poisson_distribution<> number_of_children_;
  normal_distribution<> age_of_delivery_;
  bernoulli_distribution gender_distribution_;
  int min_age_;
  int max_age_;
};

struct environment
{
  month_iterator current = date{ 1991, Sep, 1 };
  birth_distribution childbirth{ 2, 26 };
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

  generate_population(env, pd, 1000, led, sd);

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
