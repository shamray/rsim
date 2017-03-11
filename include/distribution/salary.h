#pragma once

#include "person.h"
#include <random>

namespace sim
{
  class salary_distribution
  {
  public:
    salary_distribution(int mean, int sigma)
      : dist_{ m(mean, sigma * sigma), s(mean, sigma * sigma) }
    {}

    auto operator()()
    {
      return boost::numeric_cast<long>(round(dist_(utils::random::generator())));
    }

  private:
    auto m(double mean, double variance) -> double
    {
      return log(mean * mean / sqrt(variance + mean * mean));
    }

    auto s(double mean, double variance) -> double
    {
      return sqrt(log(1 + variance / (mean * mean)));
    }

  private:
    std::lognormal_distribution<> dist_;
  };
}