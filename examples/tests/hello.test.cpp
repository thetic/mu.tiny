#include "mutiny/test.hpp"

TEST_GROUP_C_WRAPPER(hello)
{
  TEST_GROUP_C_SETUP_WRAPPER(hello)
  TEST_GROUP_C_TEARDOWN_WRAPPER(hello)
};

TEST_C_WRAPPER(hello, PrintsHelloWorld)
