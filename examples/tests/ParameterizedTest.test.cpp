#include "mu/tiny/test.hpp"

// --- 1. Plain data-driven table, the classic pytest.mark.parametrize case ---

namespace {
struct SquareCase
{
  int input;
  int expected;
};
} // namespace

TEST_GROUP(NumberChecks)
{};

PARAMETERIZED_TEST(NumberChecks, square_matches_expected, SquareCase)
{
  CHECK_EQUAL(params.expected, params.input * params.input);
}
PARAMETERIZED_TEST_CASE(
    NumberChecks,
    square_matches_expected,
    two,
    (SquareCase{ 2, 4 })
)
PARAMETERIZED_TEST_CASE(
    NumberChecks,
    square_matches_expected,
    three,
    (SquareCase{ 3, 9 })
)
PARAMETERIZED_TEST_CASE(
    NumberChecks,
    square_matches_expected,
    negative_four,
    (SquareCase{ -4, 16 })
)

namespace {

class FooInterface
{
public:
  virtual const char* class_name() const = 0;
  virtual int foo(int value) const = 0;
  virtual ~FooInterface() = default;
};

class ImplA : public FooInterface
{
public:
  const char* class_name() const override { return "ImplA"; }
  int foo(int value) const override { return value + 1; }
};

class ImplB : public FooInterface
{
public:
  const char* class_name() const override { return "ImplB"; }
  int foo(int value) const override { return value + 10; }
};

struct FooParams
{
  FooInterface* sut;
  const char* expected_name;
};

// One persistent instance per implementation, handed out by address. A
// fresh-per-test SUT is also possible - construct it directly in the case
// expression instead of taking the address of a static.
FooParams impl_a_params()
{
  static ImplA impl;
  return { &impl, "ImplA" };
}

FooParams impl_b_params()
{
  static ImplB impl;
  return { &impl, "ImplB" };
}

} // namespace

#define FOO_CASES(testName)                                                    \
  PARAMETERIZED_TEST_CASE(FooContract, testName, ImplA, (impl_a_params()))     \
  PARAMETERIZED_TEST_CASE(FooContract, testName, ImplB, (impl_b_params()))

TEST_GROUP(FooContract)
{};

PARAMETERIZED_TEST(FooContract, class_name_matches, FooParams)
{
  STRCMP_EQUAL(params.expected_name, params.sut->class_name());
}
FOO_CASES(class_name_matches)

PARAMETERIZED_TEST(FooContract, foo_returns_greater_value, FooParams)
{
  CHECK(params.sut->foo(1) > 1);
}
FOO_CASES(foo_returns_greater_value)
