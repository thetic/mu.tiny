#include "ParameterizedTest.h"

#include "mu/tiny/test.hpp"

TEST_GROUP(ParameterizedTestC)
{};

PARAMETERIZED_TEST_CASE_C_WRAPPER(
    ParameterizedTestC,
    square_matches_expected,
    two,
    SquareCase,
    (SquareCase{ 2, 4 })
)
PARAMETERIZED_TEST_CASE_C_WRAPPER(
    ParameterizedTestC,
    square_matches_expected,
    three,
    SquareCase,
    (SquareCase{ 3, 9 })
)
