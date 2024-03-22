#include "src/language/error/value_or_error.h"
#include "src/math/bigint.h"
#include "src/math/numbers.h"

using afc::language::IsError;
using afc::language::ValueOrDie;
using afc::language::ValueOrError;
using afc::math::numbers::BigInt;
using afc::math::numbers::Number;

int main(int argc, const char** argv) {
  Number x = Number::FromInt64(1024);
  Number y = ValueOrDie(Number::FromInt64(1) / Number::FromInt64(3)) +
             Number::FromDouble(297.003);
  x += y;
  std::cout << x.ToString(5) << std::endl;

  Number a = (Number::FromInt64(10).Pow(BigInt::FromNumber(50)) +
              ValueOrDie(Number::FromInt64(1) /
                         Number::FromInt64(10).Pow(BigInt::FromNumber(10)))) *
             Number::FromInt64(3);
  std::cout << a.ToString(70) << std::endl;

  std::cout
      << ValueOrDie(Number::FromInt64(2) / Number::FromInt64(3)).ToString(12)
      << std::endl;
  std::cout
      << ValueOrDie(Number::FromInt64(5) / Number::FromInt64(2)).ToString(12)
      << std::endl;

  std::cout << ValueOrDie(Number::FromInt64(1000000) / Number::FromInt64(10000))
                   .ToInt32()
            << std::endl;
  std::cout << ValueOrDie(Number::FromInt64(1000001) / Number::FromInt64(10000))
                   .ToInt32()
            << std::endl;

  ValueOrError<size_t> b = Number::FromInt64(-1).ToSizeT();
  if (IsError(b)) {
    std::cerr << b << std::endl;
  } else {
    size_t value = std::get<size_t>(b);
    std::cout << value << std::endl;
  }

  return 0;
}
