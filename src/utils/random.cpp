#include "utils/random.h"
#include <random>

namespace utils::random
{
  auto generator() -> std::mt19937&
  {
    static std::random_device rd;
    static std::mt19937 gen{ rd() };

    return gen;
  }
}
