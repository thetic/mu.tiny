#include "ParameterizedTest.h"

#include "mu/tiny/test.h"

PARAMETERIZED_TEST(ParameterizedTestC, square_matches_expected, SquareCase)
{
  CHECK_EQUAL_INT(params.expected, params.input * params.input);
}
