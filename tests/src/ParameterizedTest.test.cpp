#include "mu/tiny/test/Shell.hpp"

#include "mu/tiny/test.hpp"

TEST_GROUP(ParameterizedTest)
{};

// Each case runs as its own TEST(), named "testName_id".
PARAMETERIZED_TEST(
    ParameterizedTest,
    generated_test_is_named_correctly,
    const char*
)
{
  STRCMP_EQUAL(params, mu::tiny::test::Shell::get_current()->get_name());
}
PARAMETERIZED_TEST_CASE(
    ParameterizedTest,
    generated_test_is_named_correctly,
    alpha,
    ("generated_test_is_named_correctly_alpha")
)
PARAMETERIZED_TEST_CASE(
    ParameterizedTest,
    generated_test_is_named_correctly,
    beta,
    ("generated_test_is_named_correctly_beta")
)

// Each case's expression is evaluated fresh when its TEST() runs, and the
// resulting value reaches the shared body unchanged.
namespace {
struct SquareCase
{
  int input;
  int expected;
};
} // namespace

PARAMETERIZED_TEST(ParameterizedTest, delivers_case_value, SquareCase)
{
  CHECK_EQUAL(params.expected, params.input * params.input);
}
PARAMETERIZED_TEST_CASE(
    ParameterizedTest,
    delivers_case_value,
    two,
    (SquareCase{ 2, 4 })
)
PARAMETERIZED_TEST_CASE(
    ParameterizedTest,
    delivers_case_value,
    three,
    (SquareCase{ 3, 9 })
)
PARAMETERIZED_TEST_CASE(
    ParameterizedTest,
    delivers_case_value,
    negative_four,
    (SquareCase{ -4, 16 })
)

// The same case list can be reused across several PARAMETERIZED_TEST bodies
// by writing a small local macro that expands to one PARAMETERIZED_TEST_CASE
// per case - the pattern an interface-conformance suite relies on to apply
// its checks to every implementation under test.
namespace {
int implementation_a_value()
{
  return 1;
}
int implementation_b_value()
{
  return 2;
}
} // namespace

#define REUSED_CASES(testName)                                                 \
  PARAMETERIZED_TEST_CASE(                                                     \
      ParameterizedTest, testName, ImplA, (implementation_a_value())           \
  )                                                                            \
  PARAMETERIZED_TEST_CASE(                                                     \
      ParameterizedTest, testName, ImplB, (implementation_b_value())           \
  )

PARAMETERIZED_TEST(ParameterizedTest, reused_cases_are_positive, int)
{
  CHECK(params > 0);
}
REUSED_CASES(reused_cases_are_positive)

PARAMETERIZED_TEST(ParameterizedTest, reused_cases_are_under_ten, int)
{
  CHECK(params < 10);
}
REUSED_CASES(reused_cases_are_under_ten)
