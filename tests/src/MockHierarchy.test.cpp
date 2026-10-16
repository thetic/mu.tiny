#include "MockFailureReporter.hpp"

#include "mu/tiny/test.hpp"

using mu::tiny::mock::mock;

TEST_GROUP(MockHierarchy)
{
  void teardown() override { mock().clear(); }
};

TEST(MockHierarchy, getMockSupportScope)
{
  auto* mock1 = mock().get_mock_support_scope("name");
  auto* mock2 = mock().get_mock_support_scope("differentName");

  CHECK(!mock().has_data("name"));
  CHECK(mock1 != mock2);
  CHECK_EQUAL(mock1, mock().get_mock_support_scope("name"));
  CHECK(mock1 != &mock());
}

TEST(MockHierarchy, usingTwoMockSupportsByName)
{
  mock("first").expect_one_call("boo");

  CHECK(!mock("other").expected_calls_left());
  CHECK(mock("first").expected_calls_left());

  mock("first").clear();
}

TEST(MockHierarchy, EnableDisableWorkHierarchically)
{
  mock("first");

  mock().disable();
  mock("first").expect_one_call("boo");
  CHECK(!mock("first").expected_calls_left());

  mock().enable();
  mock("first").expect_one_call("boo");
  CHECK(mock("first").expected_calls_left());

  mock("first").clear();
}

TEST(MockHierarchy, EnableDisableWorkHierarchicallyWhenSupportIsDynamicallyCreated)
{
  mock().disable();
  mock("first").expect_one_call("boo");
  CHECK(!mock("first").expected_calls_left());

  mock().enable();
  mock("second").expect_one_call("boo");
  CHECK(mock("second").expected_calls_left());

  mock().clear();
}

TEST(MockHierarchy, ExpectedCallsLeftWorksHierarchically)
{
  mock("first").expect_one_call("foobar");
  CHECK(mock().expected_calls_left());

  mock().clear();
}

TEST(MockHierarchy, checkExpectationsWorksHierarchically)
{
  FailureReporterInstaller failure_reporter_installer;

  mock("first").expect_one_call("foobar");
  mock("second").expect_one_call("helloworld");

  mock().check_expectations();
  STRCMP_CONTAINS("first::foobar", mock_failure_string().c_str());
}

TEST(MockHierarchy, ignoreOtherCallsWorksHierarchically)
{
  mock("first");
  mock().ignore_other_calls();
  mock("first").actual_call("boo");

  mock().check_expectations();
}

TEST(MockHierarchy, ignoreOtherCallsWorksHierarchicallyWhenDynamicallyCreated)
{
  mock().ignore_other_calls();
  mock("first").actual_call("boo");

  mock().check_expectations();
}

TEST(MockHierarchy, checkExpectationsWorksHierarchicallyForLastCallNotFinished)
{
  FailureReporterInstaller failure_reporter_installer;

  mock("first").expect_one_call("foobar").with_parameter("boo", 1);
  mock("first").actual_call("foobar");

  mock().check_expectations();
  STRCMP_CONTAINS("first::foobar", mock_failure_string().c_str());
}

TEST(MockHierarchy, reporterIsInheritedInHierarchicalMocks)
{
  FailureReporterInstaller failure_reporter_installer;

  mock("differentScope").actual_call("foobar");

  STRCMP_CONTAINS("differentScope::foobar", mock_failure_string().c_str());
}

TEST(MockHierarchy, ignoredExpectedCallSetupMethodsReturnSelf)
{
  mock().disable();
  auto& ignored = mock().expect_one_call("func");
  CHECK_EQUAL(&ignored, &ignored.with_name("func"));
  CHECK_EQUAL(&ignored, &ignored.with_call_order(1));
  CHECK_EQUAL(&ignored, &ignored.with_call_order(1, 2));
  CHECK_EQUAL(&ignored, &ignored.on_object(static_cast<void*>(nullptr)));
}

TEST(MockHierarchy, ignoredExpectedCallParameterMethodsReturnSelf)
{
  mock().disable();
  auto& ignored = mock().expect_one_call("func");
  CHECK_EQUAL(&ignored, &ignored.with_parameter("b", true));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("i", 1));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("u", 1U));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("l", 1L));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("ul", 1UL));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("ll", 1LL));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("ull", 1ULL));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("d", 1.0));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("d_tol", 2.1, 0.3));
  CHECK_EQUAL(&ignored, &ignored.with_parameter("s", "hello"));
  CHECK_EQUAL(
      &ignored, &ignored.with_parameter("p", static_cast<void*>(nullptr))
  );
  CHECK_EQUAL(
      &ignored, &ignored.with_parameter("cp", static_cast<const void*>(nullptr))
  );
  CHECK_EQUAL(
      &ignored, &ignored.with_parameter("fp", static_cast<void (*)()>(nullptr))
  );
  CHECK_EQUAL(
      &ignored,
      &ignored.with_parameter(
          "mb", static_cast<const unsigned char*>(nullptr), 0
      )
  );
  CHECK_EQUAL(&ignored, &ignored.with_unmodified_output_parameter("unmod"));
  int captured = 0;
  CHECK_EQUAL(
      &ignored,
      &ignored.with_captured_parameter("c", &captured, sizeof(captured))
  );
  CHECK_EQUAL(
      &ignored,
      &ignored.with_captured_parameter_of_type("type", "cot", &captured)
  );
  CHECK_EQUAL(&ignored, &ignored.ignore_other_parameters());
}

TEST(MockHierarchy, ignoredExpectedCallAndReturnValueOverloadsReturnSelf)
{
  mock().disable();
  auto& ignored = mock().expect_one_call("func");
  CHECK_EQUAL(&ignored, &ignored.and_return_value(true));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1U));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1L));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1UL));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1LL));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1ULL));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(1.0));
  CHECK_EQUAL(&ignored, &ignored.and_return_value("boo"));
  CHECK_EQUAL(&ignored, &ignored.and_return_value(static_cast<void*>(nullptr)));
  CHECK_EQUAL(
      &ignored, &ignored.and_return_value(static_cast<const void*>(nullptr))
  );
  CHECK_EQUAL(
      &ignored, &ignored.and_return_value(static_cast<void (*)()>(nullptr))
  );
}
