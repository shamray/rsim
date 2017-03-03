#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>

struct person
{
  boost::gregorian::date birth_date;
};

struct environment
{
  double annual_birth_rate = 0;
};

int main()
{
  std::cout << "hello world";
}