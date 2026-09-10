#include "MockFailureReporter.hpp"

#include "mu/tiny/mock.hpp"
#include "mu/tiny/test.hpp"

using mu::tiny::mock::mock;

TEST_GROUP(Support)
{
  FailureReporterInstaller failure_reporter_installer;

  void teardown() override
  {
    mock().check_expectations();
    CHECK_NO_MOCK_FAILURE();
    FailureReporterForTest::clear_reporter();
    mock().clear();
  }
};

TEST(Support, setDataForUnsignedIntegerValues)
{
  unsigned int expected_data = 7;
  mock().set_data("data", expected_data);
  CHECK_EQUAL(expected_data, mock().get_data("data").get_value<unsigned int>());
}

TEST(Support, setDataForIntegerValues)
{
  int expected_data = 10;
  mock().set_data("data", expected_data);
  CHECK_EQUAL(expected_data, mock().get_data("data").get_value<int>());
}

TEST(Support, setDataForBooleanValues)
{
  bool expected_data = true;
  mock().set_data("data", expected_data);
  CHECK_EQUAL(expected_data, mock().get_data("data").get_value<bool>());
}

TEST(Support, hasDataBeenSet)
{
  CHECK(!mock().has_data("data"));
  mock().set_data("data", 10);
  CHECK(mock().has_data("data"));
}

TEST(Support, dataCanBeChanged)
{
  mock().set_data("data", 10);
  mock().set_data("data", 15);
  CHECK_EQUAL(15, mock().get_data("data").get_value<int>());
}

TEST(Support, uninitializedData)
{
  CHECK_EQUAL(0, mock().get_data("nonexisting").get_value<int>());
  STRCMP_EQUAL("int", mock().get_data("nonexisting").get_type().c_str());
}

TEST(Support, setMultipleData)
{
  mock().set_data("data", 1);
  mock().set_data("data2", 10);
  CHECK_EQUAL(1, mock().get_data("data").get_value<int>());
  CHECK_EQUAL(10, mock().get_data("data2").get_value<int>());
}

TEST(Support, setDataString)
{
  mock().set_data("data", "string");
  STRCMP_EQUAL("string", mock().get_data("data").get_value<const char*>());
}

TEST(Support, setDataDouble)
{
  mock().set_data("data", 1.0);
  CHECK_APPROX(1.0, mock().get_data("data").get_value<double>(), 0.05);
}

TEST(Support, setDataLongInt)
{
  long int i = 100;
  mock().set_data("data", i);
  CHECK_EQUAL(i, mock().get_data("data").get_value<long int>());
}

TEST(Support, setDataUnsignedLongInt)
{
  unsigned long int i = 100;
  mock().set_data("data", i);
  CHECK_EQUAL(i, mock().get_data("data").get_value<unsigned long int>());
}

TEST(Support, setDataPointer)
{
  void* ptr = reinterpret_cast<void*>(0x001);
  mock().set_data("data", ptr);
  CHECK_EQUAL(ptr, mock().get_data("data").get_value<void*>());
}

TEST(Support, setConstDataPointer)
{
  const void* ptr = reinterpret_cast<const void*>(0x001);
  mock().set_data("data", ptr);
  CHECK_EQUAL(ptr, mock().get_data("data").get_value<const void*>());
}

TEST(Support, setDataFunctionPointer)
{
  auto ptr = reinterpret_cast<void (*)()>(0x001);
  mock().set_data("data", ptr);
  CHECK_EQUAL(
      ptr,
      mock()
          .get_data("data")
          .get_value<mu::tiny::mock::NamedValue::FunctionPointerValue>()
  );
}

TEST(Support, setDataObject)
{
  void* ptr = reinterpret_cast<void*>(0x001);
  mock().set_data_object("data", "type", ptr);
  CHECK_EQUAL(ptr, mock().get_data("data").get_object_pointer());
  STRCMP_EQUAL("type", mock().get_data("data").get_type().c_str());
}

TEST(Support, setDataConstObject)
{
  void* ptr = reinterpret_cast<void*>(0x011);
  mock().set_data_const_object("data", "type", ptr);
  CHECK_EQUAL(ptr, mock().get_data("data").get_const_object_pointer());
  STRCMP_EQUAL("type", mock().get_data("data").get_type().c_str());
}

TEST(Support, tracing)
{
  mock().tracing(true);

  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");
  mock("scope")
      .actual_call("foo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");
  mock().check_expectations();

  STRCMP_CONTAINS("boo", mock().get_trace_output());
  STRCMP_CONTAINS("foo", mock().get_trace_output());
}

TEST(Support, tracingWorksHierarchically)
{
  mock("scope").tracing(true);
  mock().tracing(true);

  mock().actual_call("boo");
  mock("scope").actual_call("foo");
  mock().check_expectations();

  STRCMP_CONTAINS("boo", mock().get_trace_output());
  STRCMP_CONTAINS("foo", mock().get_trace_output());
}

TEST(Support, tracingWithCallOrder)
{
  mock().tracing(true);
  mock().actual_call("func").with_call_order(5);
  STRCMP_CONTAINS("withCallOrder:5", mock().get_trace_output());
}

TEST(Support, tracingWithParameterOfType)
{
  int obj = 0;
  mock().tracing(true);
  mock().actual_call("func").with_parameter_of_type("MyType", "param", &obj);
  STRCMP_CONTAINS("MyType", mock().get_trace_output());
  STRCMP_CONTAINS("param:", mock().get_trace_output());
}

TEST(Support, tracingWithCapturedParameter)
{
  int obj = 0;
  mock().tracing(true);
  mock().actual_call("func").with_captured_parameter("param", &obj);
  STRCMP_CONTAINS("param:", mock().get_trace_output());
}

TEST(Support, tracingWithCapturedParameterOfType)
{
  int obj = 0;
  mock().tracing(true);
  mock().actual_call("func").with_captured_parameter_of_type(
      "MyType", "param", &obj
  );
  STRCMP_CONTAINS("MyType", mock().get_trace_output());
  STRCMP_CONTAINS("param:", mock().get_trace_output());
}

TEST(Support, tracingOnObject)
{
  int obj = 0;
  mock().tracing(true);
  mock().actual_call("func").on_object(&obj);
  STRCMP_CONTAINS("onObject:", mock().get_trace_output());
}

TEST(Support, tracingHasReturnValueReturnsFalse)
{
  mock().tracing(true);
  auto& trace = mock().actual_call("func");
  CHECK(!trace.has_return_value());
}

TEST(Support, tracingReturnValueReturnsEmpty)
{
  mock().tracing(true);
  auto& trace = mock().actual_call("func");
  auto rv = trace.return_value();
  STRCMP_EQUAL("", rv.get_name().c_str());
}

TEST(Support, ignoredCallAcceptsAllParameterTypes)
{
  int obj = 0;
  unsigned char buf[] = { 0x01, 0x02 };
  mock().ignore_other_calls();
  mock()
      .actual_call("anything")
      .with_parameter("s", "hello")
      .with_parameter("p", &obj)
      .with_parameter("buf", buf, sizeof(buf))
      .with_parameter_of_type("MyType", "t", &obj)
      .on_object(&obj);
}

TEST(Support, ignoredActualCallWithNameAndCallOrderReturnSelf)
{
  mock().ignore_other_calls();
  auto& ignored = mock().actual_call("func");
  CHECK_EQUAL(&ignored, &ignored.with_name("func"));
  CHECK_EQUAL(&ignored, &ignored.with_call_order(0));
}

TEST(Support, checkedActualCallWithNameAndCallOrderUseBaseClassNoOps)
{
  mock().expect_one_call("func");
  auto& call = mock().actual_call("func");
  CHECK_EQUAL(&call, &call.with_name("func"));
  CHECK_EQUAL(&call, &call.with_call_order(1));
}

TEST(Support, ignoredCallReturnValueReturnsEmpty)
{
  mock().ignore_other_calls();
  auto& ignored = mock().actual_call("func");
  auto rv = ignored.return_value();
  STRCMP_EQUAL("", rv.get_name().c_str());
}

TEST(Support, expectedCallWithMatchingObjectPasses)
{
  void* obj = reinterpret_cast<void*>(0x001);
  mock().expect_one_call("boo").on_object(obj);
  mock().actual_call("boo").on_object(obj);
}

TEST(Support, callToStringWithRangeCallOrder)
{
  mock().expect_one_call("func").with_call_order(1, 3);
  mock().check_expectations();
  STRCMP_CONTAINS(
      "expected calls order: <1..3>",
      FailureReporterForTest::get_reporter()->mock_failure_string.c_str()
  );
  clear_mock_failure();
}

TEST(Support, callToStringWithMultipleInputParameters)
{
  mock().expect_one_call("func").with_parameter("a", 1).with_parameter("b", 2);
  mock().check_expectations();
  STRCMP_CONTAINS(
      "int a:",
      FailureReporterForTest::get_reporter()->mock_failure_string.c_str()
  );
  clear_mock_failure();
}

TEST(Support, callToStringWithInputAndOutputParameters)
{
  int val = 0;
  mock()
      .expect_one_call("func")
      .with_parameter("in", 1)
      .with_output_parameter_returning("out", &val, sizeof(val));
  mock().check_expectations();
  STRCMP_CONTAINS(
      "out", FailureReporterForTest::get_reporter()->mock_failure_string.c_str()
  );
  clear_mock_failure();
}

TEST(Support, callToStringWithMultipleOutputParameters)
{
  int val = 0;
  mock()
      .expect_one_call("func")
      .with_output_parameter_returning("out1", &val, sizeof(val))
      .with_output_parameter_returning("out2", &val, sizeof(val));
  mock().check_expectations();
  STRCMP_CONTAINS(
      "out2",
      FailureReporterForTest::get_reporter()->mock_failure_string.c_str()
  );
  clear_mock_failure();
}

TEST(Support, unexpectedAdditionalCallReportsOrdinal)
{
  mock().expect_one_call("foo");
  mock().actual_call("foo");
  mock().actual_call("foo");
  STRCMP_CONTAINS(
      "additional",
      FailureReporterForTest::get_reporter()->mock_failure_string.c_str()
  );
  clear_mock_failure();
}

TEST(Support, missingParametersToStringWithMultipleInputParameters)
{
  mock().expect_one_call("func").with_parameter("a", 1).with_parameter("b", 2);
  mock().actual_call("func");
  mock().check_expectations();
  clear_mock_failure();
}

TEST(Support, missingParametersToStringWithInputAndOutputParameters)
{
  int val = 0;
  mock()
      .expect_one_call("func")
      .with_parameter("a", 1)
      .with_output_parameter_returning("out", &val, sizeof(val));
  mock().actual_call("func");
  mock().check_expectations();
  clear_mock_failure();
}

TEST(Support, actualCallOnNonMatchingObjectGeneratesFailure)
{
  void* obj1 = reinterpret_cast<void*>(0x001);
  void* obj2 = reinterpret_cast<void*>(0x002);
  mock().expect_one_call("boo").on_object(obj1);
  mock().actual_call("boo").on_object(obj2);
  clear_mock_failure();
  mock().clear();
}

TEST(Support, discardingPriorMatchRemovesStillFinalizedExpectation)
{
  mock().expect_one_call("func").with_parameter("a", 1);
  mock().expect_one_call("func").with_parameter("a", 1);
  mock().actual_call("func").with_parameter("a", 1).with_parameter("b", 2);
  clear_mock_failure();
  mock().clear();
}

TEST(Support, parametersAfterFailedCallAreIgnored)
{
  void* obj = reinterpret_cast<void*>(0x001);
  mock().actual_call("nonexistent").with_parameter("a", 1).on_object(obj);
  clear_mock_failure();
}

TEST(Support, outputParameterAfterFailedCallIsNotWritten)
{
  int val = 99;
  mock().actual_call("nonexistent").with_output_parameter("out", &val);
  CHECK_EQUAL(99, val);
  clear_mock_failure();
}

TEST(Support, withParameterOfTypeStringOverloadNoComparatorFails)
{
  int obj = 0;
  mock().expect_one_call("func");
  mock().actual_call("func").with_parameter_of_type(
      mu::tiny::String("MyType"), mu::tiny::String("param"), &obj
  );
  clear_mock_failure();
}

TEST(Support, returnValueAfterFailedCallReturnsEmpty)
{
  auto rv = mock().actual_call("nonexistent").return_value();
  STRCMP_EQUAL("no return value", rv.get_name().c_str());
  clear_mock_failure();
}

TEST(Support, threeOutputParametersAllCopied)
{
  int a = 1;
  int b = 2;
  int c = 3;
  mock()
      .expect_one_call("func")
      .with_output_parameter_returning("a", &a, sizeof(a))
      .with_output_parameter_returning("b", &b, sizeof(b))
      .with_output_parameter_returning("c", &c, sizeof(c));

  int out_a = 0;
  int out_b = 0;
  int out_c = 0;
  mock()
      .actual_call("func")
      .with_output_parameter("a", &out_a)
      .with_output_parameter("b", &out_b)
      .with_output_parameter("c", &out_c);

  CHECK_EQUAL(1, out_a);
  CHECK_EQUAL(2, out_b);
  CHECK_EQUAL(3, out_c);
}
