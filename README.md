# Edge numbers library - Example code

## Introduction

The Edge numbers module (build target //src/math:numbers)
is a simple C++20 library implementing arithmetic operations
with arbitrary precision.

The library exposes an opaque `Number` type and implements:

* Custom overloads for arithmetic operators.
* Conversion functions to and from standard C++ types.

## Operations on Numbers

A Number instance represents a tree of operations
starting from the basic C++ types
—such as 1024 + (1 / 3) + 297.003—
in full fidelity:

    Number x = FromInt(1024);
    Number y = FromInt(1) / FromInt(3) + FromDouble(297.003);
    x += y;

## Conversions

The desired precision is specified only
when numbers are converted to strings for printing
(as well as for comparison operators).
The tree of operations is only evaluated at this point:

    ToString(x, 5);  // The string "1321.33633".

## Large numbers with small decimals

There are no limits imposed on the size of the numbers represented.
Technically, `ToString` receives the desired number of digits as a `size_t`,
so that would be the limit (numbers with 18446744073709551615 digits).

    Number a =
        (Pow(FromInt(10), 50) + FromInt(1) / Pow(FromInt(10), 10)) * FromInt(3);
    ToString(a, 70);
    // => "300000000000000000000000000000000000000000000000000.0000000003"

## Performance

Operations on `Number` instances are obviously slower
than on standard C++ types.

This is a deliberate choice for the situations where correctness and simplicity
(ensuring that every digit printed is fully accurate
and imposing no constraints
to the size of representable numbers nor decimal values)
is preferable.
I believe this to be a good choice for high-level extension languages,
that offload resource-intense computations to their host language.

The internal representation uses `std::shared_ptr`
to hold deeply immutable types,
avoiding deep copies.

## Integers

`ToString` only emits as many digits as are needed
to represent a number exactly,
up to the desired precision:

    size_t digits = 12;
    ToString(FromInt(2) / FromInt(3), digits);  // => "0.666666666666"
    ToString(FromInt(5) / FromInt(2), digits);  // => "0.25"

In the same vein,
the library can determine (with full accuracy) whether a number is an integer
(i.e. can be represented without using decimals):

  ToInt(FromInt(1000000) / FromInt(10000))  // => 100
  ToInt(FromInt(1000001) / FromInt(10000))
  // Error("Error:Inexact numbers can't be represented as integer.")

This allows Edge's extension language to use a single type to represent numbers.

## Usage

This repository is meant to be built with `bazel`:

    bazel run main
