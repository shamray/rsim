#include "utils/random.h"
#include <random>

namespace utils { namespace random
{
  auto generator() -> std::mt19937&
  {
    static std::random_device rd;
    static std::mt19937 gen{ rd() };

    return gen;
  }

}}
