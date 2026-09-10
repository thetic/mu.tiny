/**
 * @file
 * @brief Primary test-definition header for mutiny.
 *
 * Include this header in every C++ test file. It provides the macros needed
 * to declare test groups and individual tests: @ref TEST_GROUP, @ref TEST, @ref
 * SKIPPED_TEST, @ref XFAIL_TEST, and the C-interop wrappers. Assertion
 * macros are provided by @ref Shell.hpp, which this header includes
 * transitively.
 */

#ifndef INCLUDED_MU_TINY_TEST_HPP
#define INCLUDED_MU_TINY_TEST_HPP

#include "mu/tiny/test/ExpectFailShell.hpp"
#include "mu/tiny/test/Failure.hpp"
#include "mu/tiny/test/GroupLocation.hpp"
#include "mu/tiny/test/Installer.hpp"
#include "mu/tiny/test/Ordered.hpp"
#include "mu/tiny/test/Plugin.hpp"
#include "mu/tiny/test/Result.hpp"
#include "mu/tiny/test/SetPointerPlugin.hpp"
#include "mu/tiny/test/Shell.hpp"
#include "mu/tiny/test/ShellPointerArray.hpp"
#include "mu/tiny/test/SkippedShell.hpp"
#include "mu/tiny/test/Test.hpp"

/**
 * @brief Define a named test group using mu::tiny::test::Test as the base
 * class.
 *
 * All tests in the group share the same @ref mu::tiny::test::Test::setup() and
 * @ref mu::tiny::test::Test::teardown() methods.
 * @ref mu::tiny::test::Test::setup() runs before each test body; @ref
 * mu::tiny::test::Test::teardown() runs after, even if the test fails. A group
 * body may also declare helper member variables and methods accessible to all
 * tests in the group.
 *
 * @param testGroup  Identifier for the group (used in filtering and output).
 */
#define TEST_GROUP(testGroup)                                                  \
  extern mu::tiny::test::GroupLocation externTestGroup##testGroup;             \
  mu::tiny::test::GroupLocation externTestGroup##testGroup(                    \
      #testGroup, __FILE__, __LINE__                                           \
  );                                                                           \
  struct TEST_GROUP_##MutinyGroup##testGroup : public mu::tiny::test::Test

/**
 * @brief Define an individual test.
 *
 * The test body follows this macro in braces. @ref
 * mu::tiny::test::Test::setup() is called first, then the body, then @ref
 * mu::tiny::test::Test::teardown(). A test failure aborts the body but still
 * calls @ref mu::tiny::test::Test::teardown().
 *
 * @param testGroup  The group this test belongs to (must be declared first).
 * @param testName   Unique name within the group.
 */
#define TEST(testGroup, testName)                                              \
  /* External declarations for strict compilers */                             \
  class TEST_##testGroup##_##testName##_TestShell;                             \
  extern TEST_##testGroup##_##testName##_TestShell                             \
      TEST_##testGroup##_##testName##_TestShell_instance;                      \
                                                                               \
  class TEST_##testGroup##_##testName##_Test                                   \
    : public TEST_GROUP_##MutinyGroup##testGroup                               \
  {                                                                            \
  public:                                                                      \
    TEST_##testGroup##_##testName##_Test() noexcept                            \
      : TEST_GROUP_##MutinyGroup##testGroup()                                  \
    {                                                                          \
    }                                                                          \
    void test_body() override;                                                 \
  };                                                                           \
  class TEST_##testGroup##_##testName##_TestShell                              \
    : public mu::tiny::test::Shell                                             \
  {                                                                            \
    virtual mu::tiny::test::Test* create_test() override                       \
    {                                                                          \
      return new TEST_##testGroup##_##testName##_Test;                         \
    }                                                                          \
  } TEST_##testGroup##_##testName##_TestShell_instance;                        \
  namespace {                                                                  \
  mu::tiny::test::Installer TEST_##testGroup##_##testName##_Installer(         \
      TEST_##testGroup##_##testName##_TestShell_instance,                      \
      #testGroup,                                                              \
      #testName,                                                               \
      __FILE__,                                                                \
      __LINE__                                                                 \
  );                                                                           \
  } /* namespace */                                                            \
  void TEST_##testGroup##_##testName##_Test::test_body()

/**
 * @brief Define a test that is skipped during a test run.
 *
 * The test is registered and counted as "ignored" but its body is never
 * executed. Useful for temporarily disabling a test without deleting it.
 *
 * @param testGroup  The group this test belongs to.
 * @param testName   Name of the test.
 */
#define SKIPPED_TEST(testGroup, testName)                                      \
  /* External declarations for strict compilers */                             \
  class SKIP##testGroup##_##testName##_TestShell;                              \
  extern SKIP##testGroup##_##testName##_TestShell                              \
      SKIP##testGroup##_##testName##_TestShell_instance;                       \
                                                                               \
  class SKIP##testGroup##_##testName##_Test                                    \
    : public TEST_GROUP_##MutinyGroup##testGroup                               \
  {                                                                            \
  public:                                                                      \
    SKIP##testGroup##_##testName##_Test() noexcept                             \
      : TEST_GROUP_##MutinyGroup##testGroup()                                  \
    {                                                                          \
    }                                                                          \
                                                                               \
  public:                                                                      \
    void test_body() override;                                                 \
  };                                                                           \
  class SKIP##testGroup##_##testName##_TestShell                               \
    : public mu::tiny::test::SkippedShell                                      \
  {                                                                            \
    virtual mu::tiny::test::Test* create_test() override                       \
    {                                                                          \
      return new SKIP##testGroup##_##testName##_Test;                          \
    }                                                                          \
  } SKIP##testGroup##_##testName##_TestShell_instance;                         \
  namespace {                                                                  \
  mu::tiny::test::Installer TEST_##testGroup##testName##_Installer(            \
      SKIP##testGroup##_##testName##_TestShell_instance,                       \
      #testGroup,                                                              \
      #testName,                                                               \
      __FILE__,                                                                \
      __LINE__                                                                 \
  );                                                                           \
  } /* namespace */                                                            \
  void SKIP##testGroup##_##testName##_Test::test_body()

/**
 * @brief Define a test that is expected to fail.
 *
 * If the test body passes (no assertion failure), the outer run reports an
 * error. Use this to document known failures that must stay failing — the
 * test run breaks immediately if the code is accidentally fixed without
 * removing the @ref XFAIL_TEST declaration.
 *
 * @param testGroup  The group this test belongs to.
 * @param testName   Name of the test.
 */
#define XFAIL_TEST(testGroup, testName)                                        \
  /* External declarations for strict compilers */                             \
  class EXPECT_FAIL##testGroup##_##testName##_TestShell;                       \
  extern EXPECT_FAIL##testGroup##_##testName##_TestShell                       \
      EXPECT_FAIL##testGroup##_##testName##_TestShell_instance;                \
                                                                               \
  class EXPECT_FAIL##testGroup##_##testName##_Test                             \
    : public TEST_GROUP_##MutinyGroup##testGroup                               \
  {                                                                            \
  public:                                                                      \
    EXPECT_FAIL##testGroup##_##testName##_Test() noexcept                      \
      : TEST_GROUP_##MutinyGroup##testGroup()                                  \
    {                                                                          \
    }                                                                          \
    void test_body() override;                                                 \
  };                                                                           \
  class EXPECT_FAIL##testGroup##_##testName##_TestShell                        \
    : public mu::tiny::test::ExpectFailShell                                   \
  {                                                                            \
    virtual mu::tiny::test::Test* create_test() override                       \
    {                                                                          \
      return new EXPECT_FAIL##testGroup##_##testName##_Test;                   \
    }                                                                          \
  } EXPECT_FAIL##testGroup##_##testName##_TestShell_instance;                  \
  namespace {                                                                  \
  mu::tiny::test::Installer EXPECT_FAIL##testGroup##_##testName##_Installer(   \
      EXPECT_FAIL##testGroup##_##testName##_TestShell_instance,                \
      #testGroup,                                                              \
      #testName,                                                               \
      __FILE__,                                                                \
      __LINE__                                                                 \
  );                                                                           \
  } /* namespace */                                                            \
  void EXPECT_FAIL##testGroup##_##testName##_Test::test_body()

/**
 * @brief Define the shared body of a parameterized test.
 *
 * Write the body once, then list each case with @ref PARAMETERIZED_TEST_CASE.
 * Every case expands to an ordinary @ref TEST() named "testName_id" -
 * independently registered, filterable by name, and reported on failure like
 * any other test. A case failing does not affect its siblings.
 *
 * @param testGroup   An already-declared @ref TEST_GROUP.
 * @param testName    Base name; each case run as "testName_id".
 * @param ParamsType  Type passed by value to the body as @c params.
 *
 * @code{.cpp}
 * struct SquareCase { int input; int expected; };
 *
 * PARAMETERIZED_TEST(NumberChecks, square_matches_expected, SquareCase)
 * {
 *   CHECK_EQUAL(params.expected, params.input * params.input);
 * }
 * PARAMETERIZED_TEST_CASE(NumberChecks, square_matches_expected, two,
 *     (SquareCase{2, 4}))
 * PARAMETERIZED_TEST_CASE(NumberChecks, square_matches_expected, three,
 *     (SquareCase{3, 9}))
 * @endcode
 *
 * @see PARAMETERIZED_TEST_CASE
 */
#define PARAMETERIZED_TEST(testGroup, testName, ParamsType)                    \
  void testGroup##_##testName##_body(ParamsType params)

/**
 * @brief Register one case of a parameterized test.
 *
 * Must follow the matching @ref PARAMETERIZED_TEST definition in the same
 * translation unit.
 *
 * @param testGroup  Same group passed to the matching @ref PARAMETERIZED_TEST.
 * @param testName   Same name passed to the matching @ref PARAMETERIZED_TEST.
 * @param id         Short, unique-within-testName identifier for this case;
 *                   becomes the "_id" suffix of the generated test name.
 * @param expr       Expression producing a ParamsType value for this case.
 *
 * @see PARAMETERIZED_TEST
 */
#define PARAMETERIZED_TEST_CASE(testGroup, testName, id, expr)                 \
  TEST(testGroup, testName##_##id)                                             \
  {                                                                            \
    testGroup##_##testName##_body(expr);                                       \
  }

/**
 * @brief Attach a key/value property to the currently running test.
 *
 * Properties appear in JUnit XML output inside the @c \<properties\> element,
 * making it easy to annotate tests with environment data or feature flags.
 *
 * @param name   Property name string literal or const char*.
 * @param value  Property value string literal or const char*.
 */
#define TEST_PROPERTY(name, value)                                             \
  mu::tiny::test::Shell::get_current()->add_test_property((name), (value))

/**
 * @brief Declare a test group that bridges C setup/teardown functions.
 *
 * Equivalent to @ref TEST_GROUP but additionally declares the <tt>extern
 * "C"</tt> setup and teardown wrapper symbols expected by @ref
 * TEST_GROUP_C_SETUP_WRAPPER and @ref TEST_GROUP_C_TEARDOWN_WRAPPER.
 *
 * @param group_name  Name of the group (also forms the C wrapper symbol names).
 */
#define TEST_GROUP_C_WRAPPER(group_name)                                       \
  extern "C" void group_##group_name##_setup_wrapper_c(void);                  \
  extern "C" void group_##group_name##_teardown_wrapper_c(void);               \
  TEST_GROUP(group_name)

/**
 * @brief Define the C setup wrapper for a group declared with
 * @ref TEST_GROUP_C_WRAPPER.
 *
 * The body you write delegates to the C function
 * @c group_\#\#group_name\#\#_setup_wrapper_c, which is defined in the .c file.
 *
 * @param group_name  Group name, must match the @ref TEST_GROUP_C_WRAPPER
 * declaration.
 */
#define TEST_GROUP_C_SETUP_WRAPPER(group_name)                                 \
  void setup() override                                                        \
  {                                                                            \
    group_##group_name##_setup_wrapper_c();                                    \
  }

/**
 * @brief Define the C teardown wrapper for a group declared with
 * @ref TEST_GROUP_C_WRAPPER.
 *
 * @param group_name  Group name, must match the @ref TEST_GROUP_C_WRAPPER
 * declaration.
 */
#define TEST_GROUP_C_TEARDOWN_WRAPPER(group_name)                              \
  void teardown() override                                                     \
  {                                                                            \
    group_##group_name##_teardown_wrapper_c();                                 \
  }

/**
 * @brief Bridge a C test function into a TEST.
 *
 * Creates a @ref TEST whose body calls the C function
 * @c test_\#\#group_name\#\#_\#\#test_name\#\#_wrapper_c. The corresponding
 * C-side definition uses @ref TEST() from @ref mu/tiny/test.h in the .c file.
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define TEST_C_WRAPPER(group_name, test_name)                                  \
  extern "C" void test_##group_name##_##test_name##_wrapper_c();               \
  TEST(group_name, test_name)                                                  \
  {                                                                            \
    test_##group_name##_##test_name##_wrapper_c();                             \
  }

/**
 * @brief Bridge a C @ref SKIPPED_TEST into an @ref SKIPPED_TEST.
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define SKIPPED_TEST_C_WRAPPER(group_name, test_name)                          \
  extern "C" void ignore_##group_name##_##test_name##_wrapper_c();             \
  SKIPPED_TEST(group_name, test_name)                                          \
  {                                                                            \
    ignore_##group_name##_##test_name##_wrapper_c();                           \
  }

/**
 * @brief Bridge a C @ref XFAIL_TEST into an @ref XFAIL_TEST.
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define XFAIL_TEST_C_WRAPPER(group_name, test_name)                            \
  extern "C" void expect_fail_##group_name##_##test_name##_wrapper_c();        \
  XFAIL_TEST(group_name, test_name)                                            \
  {                                                                            \
    expect_fail_##group_name##_##test_name##_wrapper_c();                      \
  }

#endif
