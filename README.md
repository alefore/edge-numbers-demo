# Edge numbers library - Example code

## Introduction

The [Edge](http://github.com/alefore/edge) numbers module
(build target `//src/math:numbers` in Edge)
is a simple C++20 library implementing arithmetic operations
with arbitrary precision.

The library exposes an opaque `Number` type and implements:

* Custom overloads for arithmetic operators.
* Conversion functions to and from standard C++ types
  (currently supporting `int`, `size_t` and `double`).

`Number` instances hold internally a representation of a tree of operations.

### Large numbers with small decimals

No limits are imposed to the size of the numbers represented.

    Number a =
        (Pow(FromInt(10), 50) + FromInt(1) / Pow(FromInt(10), 10)) * FromInt(3);
    ToString(a, 70);
    // => "300000000000000000000000000000000000000000000000000.0000000003"

Technically, `ToString` receives the desired number of digits as a `size_t`.
It would be more accurate to say that
you may have trouble getting the decimal parts
of numbers above 10¹⁸⁴⁴⁶⁷⁴⁴⁰⁷³⁷⁰⁹⁵⁵¹⁶¹⁵.

### When should I avoid this?

For resource-intense computations, there are probably better choices.
Operations on `Number` instances are slower than on standard C++ types.
Because `Number` retains the tree of operations (and evaluates it lazily),
memory could also be a concern.

See also the [list of caveats](#caveats).

### When should I use this?

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

## Programming interface

### Number operations

A `Number` instance represents a tree of operations
—such as 1024 + (1 / 3) + 297.003—
starting from the basic C++ types.

    Number x = FromInt(1024);
    Number y = FromInt(1) / FromInt(3) + FromDouble(297.003);
    x += y;

`Number` retains the tree of operations
with full fidelity
in deeply immutable instances
held through  `std::shared_ptr<>`
to avoid deep copies
(at the cost of incurring spurious increment/decrement operations,
unless `std::move()` is used).

### Conversions to string

The desired precision is specified only
when numbers are converted to strings for printing
(as well as for comparison operators).

    ToString(x, 5);  // The string "1321.33633".

The tree of operations is only evaluated at this point.
This library is, in that sense, "lazy".
Division-by-zero errors are only detected during conversion.

In the future, we may optimize the implementation
to evaluate operations eagerly
when no precision is lost
(which we expect to be the majority of cases).

One can, of course, force eager evaluation by converting numbers
into standard C++ types and back into `Number` instances.

### Exact numbers

`ToString` only emits as many digits as needed
to represent a number exactly,
up to the desired precision:

    size_t digits = 12;
    ToString(FromInt(2) / FromInt(3), digits);  // => "0.666666666666"
    ToString(FromInt(5) / FromInt(2), digits);  // => "0.25"

In the same vein,
the library can determine with full accuracy
whether a number is an integer
(i.e. can be represented without using decimals):

    ToInt(FromInt(1000000) / FromInt(10000))  // => 100
    ToInt(FromInt(1000001) / FromInt(10000))
    // => Error("Error:Inexact numbers can't be represented as integer.")

This allows Edge's extension language to use a single type to represent numbers.

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

## Usage example

The [edge-numbers-demo repository](http://github.com/alefore/edge-numbers-demo)
is meant to be built with `bazel`:

    bazel run main

## Implementation

The implementation is mostly in the following files,
which are part of the Edge repository:

* Header file:
  https://github.com/alefore/edge/blob/master/src/math/numbers.h
* Implementation and tests:
  https://github.com/alefore/edge/blob/master/src/math/numbers.cc

As of 2023-10-03, the implementation file is ~600 LOC, ~200 of which are tests.

## Caveats

Currently, conversion function `FromDouble`
only retains some of the decimal points from the input.
This could be improved significantly.

This library is fairly new and doesn't have as large testing coverage.
