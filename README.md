# Edge numbers library - Example code

## Introduction

The [Edge](http://github.com/alefore/edge) numbers module
(build target `//src/math:numbers` in Edge)
is a C++23 library implementing arithmetic operations
with arbitrary precision.

The library exposes an opaque `Number` type and implements:

* Custom overloads for various arithmetic operators.
* Conversion functions to and from standard C++ types
  (currently supporting `int64_t`, `size_t` and `double`).

## Quick demo

The [edge-numbers-demo repository](http://github.com/alefore/edge-numbers-demo)
contains two build targets
meant to be built with `bazel`:

* [calculator](https://github.com/alefore/edge-numbers-demo/blob/main/calculator.cc):
  A very simple CLI interface
  implementing a calculator
  that uses this library under-the-hood.

* [main](https://github.com/alefore/edge-numbers-demo/blob/main/main.cc):
  A few examples of how the API is used from C++.

## Example

### Programming Interface

    Number x = Number::FromInt64(1024);
    Number y = ValueOrDie(Number::FromInt64(1) / Number::FromInt64(3)) +
               Number::FromDouble(297.003);
    x += y;

### Calculator

    $ bazel run calculator
    1 / 3 * 100000 * 10009999 + 1
    333666633334.33333
    >>> number fact(number x) { if (x <= 1) return 1; else return x * fact(x - 1); }
    >>> <void>
    >>> fact(150)
    >>> 57133839564458545904789328652610540031895535786011264182548375833179829124845398393126574488675311145377107878746854204162666250198684504466355949195922066574942592095735778929325357290444962472405416790722118445437122269675520000000000000000000000000000000000000

## Conversion to string

The desired precision is specified only
when numbers are converted to strings for printing.

    std::cout << x.ToString(x) << std::endl;  // The string "1321.33633".

The decimal digits parameter specifies
the *maximum* number of digits that can be used beyond the decimal point;
digits will only be added if need.
For example, `1 / 2` will be represented as `0.5`
even if decimal digits is larger than 1.

    size_t digits = 12;

    (Number::FromInt64(2) / Number::FromInt64(3)).ToString(digits);
    // => "0.666666666667"

    (Number::FromInt64(5) / Number::FromInt64(2)).ToString(digits);
    // => "2.5"

## Large numbers with small decimals

No limits are imposed to the size nor precision of the numbers represented.

    Number a = (Number::FromInt64(10).Pow(BigInt::FromNumber(50)) +
                ValueOrDie(Number::FromInt64(1) /
                           Number::FromInt(10).Pow(BigInt::FromNumber(10)))) *
               Number::FromInt64(3);
    a.ToString(70);
    // => "300000000000000000000000000000000000000000000000000.0000000003"

`ToString` receives the desired number of digits as a `size_t`.
This means that you'd need to extend this code
if you wanted to get the decimal parts
of numbers above roughly 10¹⁸⁴⁴⁶⁷⁴⁴⁰⁷³⁷⁰⁹⁵⁵¹⁶¹⁵.

## Internal representation

Numbers are internally represented as a quotient
of two `BigInt` natural numbers.
`BigInt` numbers are represented as vectors of `size_t` values,
in base 10.

A `Number::Optimize` method can be used to remove common divisors
from the numerator and denominator.

## When should I _not_ use this?

For resource-intense computations, there are probably better choices.
Operations on `Number` instances are slower than on standard C++ types.

The `BigInt` instances inside a `Number` instance can grow unbounded.
Memory could eventually be a concern.
In this case, programs should call the `Number::Optimize` method.

See also the [list of caveats](#caveats).

## When should I use this?

Using `Number` should be a deliberate choice
when correctness and simplicity
(ensuring that every digit printed is fully accurate
and imposing no constraints
to the size of representable numbers nor decimal values)
are desirable.

This library is probably a good choice for high-level extension languages
(such as Edge's C-like memory-managed language),
which can offload resource-intense computations
to their host language.

## Who is using this?

As far as I know,
the only user of this library is the Edge text editor.
The extension language in Edge represents all numbers with this library.

## Programming interface

This documentation currently reflects an older version of this API.

### Creating Numbers

Use the following factory methods to create new `Number` instances:

* `Number::FromInt64`
* `Number::FromSizeT`
* `Number::FromDouble`

You can also use standard arithmetic operators
and the `Pow` method
(implementing exponentiation by a natural number).

### Conversions to string

The desired precision is specified only
when numbers are converted to strings for printing
(as well as for comparison operators).

    x.ToString(5);  // The string "1321.33633".

### Conversions to primitive types

Use the following methods to convert to standard C++ types:

* `Number::ToInt64`
* `Number::ToDouble`
* `Number::SizeT`

With the exception of `ToDouble`,
these methods will return an error
if the number can't be accurately represented in the desired type:

    Number::ToInt32(
        ValueOrError(Number::FromInt(1010000) / Number::FromInt(10000)))
    // => 101

    Number::ToInt32(
        ValueOrError(Number::FromInt(1001000) / Number::FromInt(10000)))
    // Error("Error:Non-empty remainder: 1")

### Error handling

Operations that can run into errors return the type `ValueOrError<T>`
(where `T` represents an arbitrary type).
`ValueOrError<T>` is defined as `std::variant<T, Error>`,
where `Error` is a custom class that holds an error.

[`main.cc`](https://github.com/alefore/edge-numbers-demo/blob/main/main.cc)
contains an example:

    ValueOrError<size_t> b = ToSizeT(FromInt(5) - FromInt(6));
    if (IsError(b)) {
      std::cerr << b << std::endl;
    } else {
      size_t value = std::get<size_t>(b);
      ...
    }

`ValueOrError<T>` is a prevalent pattern
applied throughout Edge to handle errors.

## Implementation

The implementation is mostly in the following files,
which are part of the Edge repository:

* `BigInt`:
  * Header file:
    https://github.com/alefore/edge/blob/master/src/math/bigint.h
  * Implementation and tests:
    https://github.com/alefore/edge/blob/master/src/math/bigint.cc
* `Number`:
  * Header file:
    https://github.com/alefore/edge/blob/master/src/math/numbers.h
  * Implementation and tests:
    https://github.com/alefore/edge/blob/master/src/math/numbers.cc

As of 2023-10-03, the implementation file is ~600 LOC, ~200 of which are tests.

## Caveats

* Currently, conversion function `FromDouble`
  only retains some of the decimal points from the input.
  This could be improved significantly.

* This library is fairly new and doesn't have as large testing coverage.
