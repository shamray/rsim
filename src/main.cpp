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

struct environment
{
  month_iterator current = date{ 1991, Sep, 1 };
  double annual_birth_rate = 0;
  long id = 0;
  unordered_map<long, person> population;
  multimap<date, function<void()>> events;

  void event_person_born()
  {}

  void event_person_died()
  {}

  environment(environment&&) = default;
  environment& operator=(environment&&) = default;
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

auto generate_population(environment& env, population_distribution& distribution, int size, life_expectancy_distribution& led, salary_distribution& sd)
{
  for (auto i = 0; i < size; ++i)
  {
    auto id = ++env.id;
    env.population[id] = distribution(*env.current);

    auto death_date = led(*env.current, env.population[id].gender);
    env.events.insert(make_pair(death_date, [&env, id]() { env.population.erase(id); }));

    auto salary = sd();
    auto work_start = utils::datetime::at_age(18, env.population[id].birth_date);
    env.events.insert(make_pair(work_start, [&env, id, salary]() { env.population[id].salary = salary; }));
  }
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

  for (; *env.current <= date{ 2075, Jan, 1 }; ++env.current)
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
