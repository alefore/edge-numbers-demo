#include "src/math/numbers.h"

using afc::math::numbers::FromDouble;
using afc::math::numbers::FromInt;
using afc::math::numbers::Number;
using afc::math::numbers::ToString;

Number Pow(Number base, size_t i) {
  Number output = FromInt(1);
  while (i > 0) {
    if (i % 2 == 1) {
      output = output * base;
      --i;
    } else {
      base *= base;
      i /= 2;
    }
  }
  return output;
}

int main(int argc, const char** argv) {
  Number x = FromInt(1024);
  Number y = FromInt(1) / FromInt(3) + FromDouble(297.003);
  x += y;
  std::cout << ToString(x, 5) << std::endl;

  Number a =
      (Pow(FromInt(10), 50) + FromInt(1) / Pow(FromInt(10), 10)) * FromInt(3);
  std::cout << ToString(a, 70) << std::endl;

  std::cout << ToString(FromInt(2) / FromInt(3), 12) << std::endl;
  std::cout << ToString(FromInt(5) / FromInt(2), 12) << std::endl;

  std::cout << ToInt(FromInt(1000000) / FromInt(10000)) << std::endl;
  std::cout << ToInt(FromInt(1000001) / FromInt(10000)) << std::endl;

  return 0;
}
