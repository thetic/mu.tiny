#include "MockFailureReporter.hpp"

#include "mu/tiny/test.hpp"

using mu::tiny::mock::mock;

TEST_GROUP(MockParameter)
{
  void teardown() override
  {
    mock().check_expectations();
    mock().clear();
  }
};

TEST(MockParameter, expectOneBooleanParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter("parameter", true);
  mock().actual_call("foo").with_parameter("parameter", true);

  mock().check_expectations();
}

TEST(MockParameter, expectOneUnsignedIntegerParameterAndValue)
{
  unsigned int value = 14400;
  mock().expect_one_call("foo").with_parameter("parameter", value);
  mock().actual_call("foo").with_parameter("parameter", value);

  mock().check_expectations();
}

TEST(MockParameter, expectOneIntegerParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter("parameter", 10);
  mock().actual_call("foo").with_parameter("parameter", 10);

  mock().check_expectations();
}

TEST(MockParameter, expectOneUnsignedLongLongIntegerParameterAndValue)
{
  unsigned long long value = 0xFFFFAAAAFFFFAAAAULL;
  mock().expect_one_call("foo").with_parameter("parameter", value);
  mock().actual_call("foo").with_parameter("parameter", value);

  mock().check_expectations();
}

TEST(MockParameter, expectOneLongLongIntegerParameterAndValue)
{
  long long value = 0x7FFFAAAAFFFFAAAAULL;

  mock().expect_one_call("foo").with_parameter("parameter", value);
  mock().actual_call("foo").with_parameter("parameter", value);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesIntAndLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1);
  mock().actual_call("foo").with_parameter("parameter", 1L);

  mock().expect_one_call("foo").with_parameter("parameter", 1L);
  mock().actual_call("foo").with_parameter("parameter", 1);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesIntAndUnsignedAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1);
  mock().actual_call("foo").with_parameter("parameter", 1U);

  mock().expect_one_call("foo").with_parameter("parameter", 1U);
  mock().actual_call("foo").with_parameter("parameter", 1);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesIntAndUnsignedLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1);
  mock().actual_call("foo").with_parameter("parameter", 1UL);

  mock().expect_one_call("foo").with_parameter("parameter", 1UL);
  mock().actual_call("foo").with_parameter("parameter", 1);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedAndLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1U);
  mock().actual_call("foo").with_parameter("parameter", 1L);

  mock().expect_one_call("foo").with_parameter("parameter", 1L);
  mock().actual_call("foo").with_parameter("parameter", 1U);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedAndUnsignedLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1U);
  mock().actual_call("foo").with_parameter("parameter", 1UL);

  mock().expect_one_call("foo").with_parameter("parameter", 1UL);
  mock().actual_call("foo").with_parameter("parameter", 1U);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesLongAndUnsignedLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1L);
  mock().actual_call("foo").with_parameter("parameter", 1UL);

  mock().expect_one_call("foo").with_parameter("parameter", 1UL);
  mock().actual_call("foo").with_parameter("parameter", 1L);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesIntAndLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1);
  mock().actual_call("foo").with_parameter("parameter", 1LL);

  mock().expect_one_call("foo").with_parameter("parameter", 1LL);
  mock().actual_call("foo").with_parameter("parameter", 1);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesIntAndUnsignedLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1);
  mock().actual_call("foo").with_parameter("parameter", 1ULL);

  mock().expect_one_call("foo").with_parameter("parameter", 1ULL);
  mock().actual_call("foo").with_parameter("parameter", 1);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedAndLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1U);
  mock().actual_call("foo").with_parameter("parameter", 1LL);

  mock().expect_one_call("foo").with_parameter("parameter", 1LL);
  mock().actual_call("foo").with_parameter("parameter", 1U);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedAndUnsignedLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1U);
  mock().actual_call("foo").with_parameter("parameter", 1ULL);

  mock().expect_one_call("foo").with_parameter("parameter", 1ULL);
  mock().actual_call("foo").with_parameter("parameter", 1U);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedLongAndUnsignedLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1UL);
  mock().actual_call("foo").with_parameter("parameter", 1ULL);

  mock().expect_one_call("foo").with_parameter("parameter", 1ULL);
  mock().actual_call("foo").with_parameter("parameter", 1UL);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesLongAndLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1L);
  mock().actual_call("foo").with_parameter("parameter", 1LL);

  mock().expect_one_call("foo").with_parameter("parameter", 1LL);
  mock().actual_call("foo").with_parameter("parameter", 1L);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesLongAndUnsignedLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1L);
  mock().actual_call("foo").with_parameter("parameter", 1ULL);

  mock().expect_one_call("foo").with_parameter("parameter", 1ULL);
  mock().actual_call("foo").with_parameter("parameter", 1L);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesUnsignedLongAndLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1UL);
  mock().actual_call("foo").with_parameter("parameter", 1LL);

  mock().expect_one_call("foo").with_parameter("parameter", 1LL);
  mock().actual_call("foo").with_parameter("parameter", 1UL);

  mock().check_expectations();
}

TEST(MockParameter, mismatchedIntegerTypesLongLongAndUnsignedLongLongAreAllowed)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1LL);
  mock().actual_call("foo").with_parameter("parameter", 1ULL);

  mock().expect_one_call("foo").with_parameter("parameter", 1ULL);
  mock().actual_call("foo").with_parameter("parameter", 1LL);

  mock().check_expectations();
}

TEST(MockParameter, longAndUnsignedLongWithSameBitRepresentationShouldNotBeTreatedAsEqual)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", -1L);
  mock().actual_call("foo").with_parameter(
      "parameter", static_cast<unsigned long>(-1)
  );

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, unsignedLongAndLongWithSameBitRepresentationShouldnotBeTreatedAsEqual)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter(
      "parameter", static_cast<unsigned long>(-1)
  );
  mock().actual_call("foo").with_parameter("parameter", -1L);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneDoubleParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter("parameter", 1.0);
  mock().actual_call("foo").with_parameter("parameter", 1.0);

  mock().check_expectations();
}

TEST(MockParameter, expectOneDoubleParameterAndValueAndTolerance)
{
  mock().expect_one_call("foo").with_parameter("parameter", 100.0, 5.0);
  mock().actual_call("foo").with_parameter("parameter", 96.0);

  mock().check_expectations();
}

TEST(MockParameter, doubleParameterNotEqualIfOutsideTolerance)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", 100.0, 5.0);
  mock().actual_call("foo").with_parameter("parameter", 106.0);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneStringParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter("parameter", "string");
  mock().actual_call("foo").with_parameter("parameter", "string");

  mock().check_expectations();
}

TEST(MockParameter, expectOnePointerParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter(
      "parameter", reinterpret_cast<void*>(0x01)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", reinterpret_cast<void*>(0x01)
  );

  mock().check_expectations();
}

TEST(MockParameter, expectOneConstPointerParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter(
      "parameter", reinterpret_cast<const void*>(0x01)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", reinterpret_cast<const void*>(0x01)
  );

  mock().check_expectations();
}

TEST(MockParameter, expectOneFunctionPointerParameterAndValue)
{
  mock().expect_one_call("foo").with_parameter(
      "parameter", reinterpret_cast<void (*)()>(0x01)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", reinterpret_cast<void (*)()>(0x01)
  );

  mock().check_expectations();
}

TEST(MockParameter, expectOneMemBufferParameterAndValue)
{
  unsigned char mem_buffer1[] = { 0x12, 0x15, 0xFF };
  unsigned char mem_buffer2[] = { 0x12, 0x15, 0xFF };
  mock().expect_one_call("foo").with_parameter(
      "parameter", mem_buffer1, sizeof(mem_buffer1)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", mem_buffer2, sizeof(mem_buffer2)
  );

  mock().check_expectations();
}

TEST(MockParameter, expectOneMemBufferParameterAndValueFailsDueToContents)
{
  FailureReporterInstaller failure_reporter_installer;

  unsigned char mem_buffer1[] = { 0x12, 0x15, 0xFF };
  unsigned char mem_buffer2[] = { 0x12, 0x05, 0xFF };

  mock().expect_one_call("foo").with_parameter(
      "parameter", mem_buffer1, sizeof(mem_buffer1)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", mem_buffer2, sizeof(mem_buffer2)
  );

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneMemBufferParameterAndValueFailsDueToSize)
{
  FailureReporterInstaller failure_reporter_installer;

  unsigned char mem_buffer1[] = { 0x12, 0x15, 0xFF };
  unsigned char mem_buffer2[] = { 0x12, 0x15, 0xFF, 0x90 };

  mock().expect_one_call("foo").with_parameter(
      "parameter", mem_buffer1, sizeof(mem_buffer1)
  );
  mock().actual_call("foo").with_parameter(
      "parameter", mem_buffer2, sizeof(mem_buffer2)
  );

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneStringParameterAndValueFails)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", "string");
  mock().actual_call("foo").with_parameter("parameter", "different");

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneUnsignedIntegerParameterAndFailsDueToParameterName)
{
  FailureReporterInstaller failure_reporter_installer;

  unsigned int value = 7;

  mock().expect_one_call("foo").with_parameter("parameter", value);
  mock().actual_call("foo").with_parameter("different", value);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneIntegerParameterAndFailsDueToParameterName)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", 10);
  mock().actual_call("foo").with_parameter("different", 10);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneUnsignedIntegerParameterAndFailsDueToValue)
{
  FailureReporterInstaller failure_reporter_installer;

  unsigned int actual_value = 8;
  unsigned int expected_value = actual_value + 1;

  mock().expect_one_call("foo").with_parameter("parameter", expected_value);
  mock().actual_call("foo").with_parameter("parameter", actual_value);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneIntegerParameterAndFailsDueToValue)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", 10);
  mock().actual_call("foo").with_parameter("parameter", 8);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectOneIntegerParameterAndFailsDueToTypes)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("parameter", 10);
  mock().actual_call("foo").with_parameter("parameter", "heh");

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, expectMultipleCallsWithDifferentParametersThatHappenOutOfOrder)
{
  mock().expect_one_call("foo").with_parameter("p1", 1);
  mock().expect_one_call("foo").with_parameter("p1", 2);
  mock().actual_call("foo").with_parameter("p1", 2);
  mock().actual_call("foo").with_parameter("p1", 1);

  mock().check_expectations();
}

TEST(MockParameter, expectMultipleCallsWithMultipleDifferentParametersThatHappenOutOfOrder)
{
  mock().expect_one_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);
  mock().expect_one_call("foo").with_parameter("p1", 1).with_parameter(
      "p2", 20
  );

  mock().actual_call("foo").with_parameter("p1", 1).with_parameter("p2", 20);
  mock().actual_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);

  mock().check_expectations();
}

TEST(MockParameter, twiceCalledWithSameParameters)
{
  mock().expect_one_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);
  mock().expect_one_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);
  mock().actual_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);
  mock().actual_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);

  mock().check_expectations();
}

TEST(MockParameter, calledWithoutParameters)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("p1", 1);
  mock().actual_call("foo");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, ignoreOtherParameters)
{
  mock()
      .expect_one_call("foo")
      .with_parameter("p1", 1)
      .ignore_other_parameters();
  mock().actual_call("foo").with_parameter("p1", 1).with_parameter("p2", 2);

  mock().check_expectations();
}

TEST(MockParameter, ignoreOtherParametersButStillPassAll)
{
  mock()
      .expect_one_call("foo")
      .with_parameter("p1", 1)
      .ignore_other_parameters();
  mock().actual_call("foo").with_parameter("p1", 1);

  mock().check_expectations();
}

TEST(MockParameter, ignoreOtherParametersButExpectedParameterDidntHappen)
{
  FailureReporterInstaller failure_reporter_installer;

  mock()
      .expect_one_call("foo")
      .with_parameter("p1", 1)
      .ignore_other_parameters();
  mock()
      .actual_call("foo")
      .with_parameter("p2", 2)
      .with_parameter("p3", 3)
      .with_parameter("p4", 4);

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, ignoreOtherParametersMultipleCalls)
{
  mock().expect_one_call("foo").ignore_other_parameters();
  mock().expect_one_call("foo").ignore_other_parameters();
  mock()
      .actual_call("foo")
      .with_parameter("p2", 2)
      .with_parameter("p3", 3)
      .with_parameter("p4", 4);
  CHECK(mock().expected_calls_left());
  mock()
      .actual_call("foo")
      .with_parameter("p2", 2)
      .with_parameter("p3", 3)
      .with_parameter("p4", 4);

  mock().check_expectations();
}

TEST(MockParameter, ignoreOtherParametersMultipleCallsButOneDidntHappen)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("boo").ignore_other_parameters();
  mock().expect_one_call("boo").ignore_other_parameters();
  mock().actual_call("boo");

  mock().check_expectations();
  STRCMP_CONTAINS("boo", mock_failure_string().c_str());
}

TEST(MockParameter, newCallStartsWhileNotAllParametersWerePassed)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_parameter("p1", 1);
  mock().actual_call("foo");
  mock().actual_call("foo").with_parameter("p1", 1);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, outputParameterSucceeds)
{
  int param = 1;
  int retval = 2;

  mock()
      .expect_one_call("function")
      .with_output_parameter_returning(
          "parameterName", &retval, sizeof(retval)
      );
  mock().actual_call("function").with_output_parameter("parameterName", &param);

  CHECK_EQUAL(param, 2);
  CHECK_EQUAL(retval, 2);
  mock().check_expectations();
}

TEST(MockParameter, unmodifiedOutputParameterSucceeds)
{
  int param = 1;

  mock()
      .expect_one_call("function")
      .with_unmodified_output_parameter("parameterName");
  mock().actual_call("function").with_output_parameter("parameterName", &param);

  CHECK_EQUAL(param, 1);
  mock().check_expectations();
}

TEST(MockParameter, noActualCallForOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  int output = 0;
  mock().expect_one_call("foo").with_output_parameter_returning(
      "output", &output, sizeof(output)
  );

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, noActualCallForUnmodifiedOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  mock().expect_one_call("foo").with_unmodified_output_parameter("output");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, unexpectedOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  int param;
  mock().expect_one_call("foo");
  mock().actual_call("foo").with_output_parameter("parameterName", &param);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, outputParameterMissing)
{
  FailureReporterInstaller failure_reporter_installer;

  int output = 0;
  mock().expect_one_call("foo").with_output_parameter_returning(
      "output", &output, sizeof(output)
  );
  mock().actual_call("foo");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, twoOutputParameters)
{
  int param1 = 55;
  int retval1 = 1;
  int param2 = 77;
  int retval2 = 2;

  mock()
      .expect_one_call("function")
      .with_output_parameter_returning(
          "parameterName", &retval1, sizeof(retval1)
      )
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_output_parameter_returning(
          "parameterName", &retval2, sizeof(retval2)
      )
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter("parameterName", &param1)
      .with_parameter("id", 1);
  mock()
      .actual_call("function")
      .with_output_parameter("parameterName", &param2)
      .with_parameter("id", 2);

  CHECK_EQUAL(retval1, param1);
  CHECK_EQUAL(retval2, param2);
  mock().check_expectations();
}

TEST(MockParameter, twoInterleavedOutputParameters)
{
  int param1 = 55;
  int retval1 = 1;
  int param2 = 77;
  int retval2 = 2;

  mock()
      .expect_one_call("function")
      .with_output_parameter_returning(
          "parameterName", &retval1, sizeof(retval1)
      )
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_output_parameter_returning(
          "parameterName", &retval2, sizeof(retval2)
      )
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter("parameterName", &param2)
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter("parameterName", &param1)
      .with_parameter("id", 1);

  CHECK_EQUAL(retval1, param1);
  CHECK_EQUAL(retval2, param2);
  mock().check_expectations();
}

TEST(MockParameter, capturedParameterSucceeds)
{
  int param = 1;
  int retval = 2;

  mock()
      .expect_one_call("function")
      .with_captured_parameter("parameterName", &param, sizeof(param));
  mock()
      .actual_call("function")
      .with_captured_parameter("parameterName", &retval);

  CHECK_EQUAL(param, 2);
  CHECK_EQUAL(retval, 2);
  mock().check_expectations();
}

TEST(MockParameter, noActualCallForCapturedParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  int captured = 0;
  mock().expect_one_call("foo").with_captured_parameter(
      "captured", &captured, sizeof(captured)
  );

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, unexpectedCapturedParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  int retval = 0;
  mock().expect_one_call("foo");
  mock().actual_call("foo").with_captured_parameter("parameterName", &retval);

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, capturedParameterMissing)
{
  FailureReporterInstaller failure_reporter_installer;

  int captured = 0;
  mock().expect_one_call("foo").with_captured_parameter(
      "captured", &captured, sizeof(captured)
  );
  mock().actual_call("foo");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());
}

TEST(MockParameter, twoCapturedParameters)
{
  int param1 = 55;
  int retval1 = 1;
  int param2 = 77;
  int retval2 = 2;

  mock()
      .expect_one_call("function")
      .with_captured_parameter("parameterName", &param1, sizeof(param1))
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_captured_parameter("parameterName", &param2, sizeof(param2))
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_captured_parameter("parameterName", &retval1)
      .with_parameter("id", 1);
  mock()
      .actual_call("function")
      .with_captured_parameter("parameterName", &retval2)
      .with_parameter("id", 2);

  CHECK_EQUAL(retval1, param1);
  CHECK_EQUAL(retval2, param2);
  mock().check_expectations();
}

TEST(MockParameter, twoInterleavedCapturedParameters)
{
  int param1 = 55;
  int retval1 = 1;
  int param2 = 77;
  int retval2 = 2;

  mock()
      .expect_one_call("function")
      .with_captured_parameter("parameterName", &param1, sizeof(param1))
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_captured_parameter("parameterName", &param2, sizeof(param2))
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_captured_parameter("parameterName", &retval2)
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_captured_parameter("parameterName", &retval1)
      .with_parameter("id", 1);

  CHECK_EQUAL(retval1, param1);
  CHECK_EQUAL(retval2, param2);
  mock().check_expectations();
}

TEST(MockParameter, twoDifferentOutputParametersInSameFunctionCallSucceeds)
{
  int param1 = 1;
  int param2 = 1;
  int retval1 = 2;
  int retval2 = 3;

  mock()
      .expect_one_call("foo")
      .with_output_parameter_returning("bar", &retval1, sizeof(retval1))
      .with_output_parameter_returning("foobar", &retval2, sizeof(retval2));
  mock()
      .actual_call("foo")
      .with_output_parameter("bar", &param1)
      .with_output_parameter("foobar", &param2);

  CHECK_EQUAL(2, retval1);
  CHECK_EQUAL(2, param1);
  CHECK_EQUAL(3, retval2);
  CHECK_EQUAL(3, param2);
  mock().check_expectations();
}

TEST(MockParameter, outputAndIntParametersOfSameNameInDifferentFunctionCallsOfSameFunctionSucceeds)
{
  int param = 1;
  int retval = 2;

  mock().expect_one_call("foo").with_output_parameter_returning(
      "bar", &retval, sizeof(retval)
  );
  mock().expect_one_call("foo").with_parameter("bar", 25);
  mock().actual_call("foo").with_output_parameter("bar", &param);
  mock().actual_call("foo").with_parameter("bar", 25);

  CHECK_EQUAL(2, retval);
  CHECK_EQUAL(2, param);
  mock().check_expectations();
}

TEST(MockParameter, twoOutputParameterOfSameNameInDifferentFunctionCallsOfSameFunctionSucceeds)
{
  int param1 = 1;
  int param2 = 1;
  int retval1 = 2;
  int retval2 = 3;

  mock().expect_one_call("foo").with_output_parameter_returning(
      "bar", &retval1, sizeof(retval1)
  );
  mock().expect_one_call("foo").with_output_parameter_returning(
      "bar", &retval2, sizeof(retval2)
  );
  mock().actual_call("foo").with_output_parameter("bar", &param1);
  mock().actual_call("foo").with_output_parameter("bar", &param2);

  CHECK_EQUAL(2, retval1);
  CHECK_EQUAL(2, param1);
  CHECK_EQUAL(3, retval2);
  CHECK_EQUAL(3, param2);
  mock().check_expectations();
}

TEST(MockParameter, twoOutputParametersOfSameNameInDifferentFunctionsSucceeds)
{
  int param = 1;
  int retval = 2;

  mock().expect_one_call("foo1").with_output_parameter_returning(
      "bar", &retval, sizeof(retval)
  );
  mock().expect_one_call("foo2").with_parameter("bar", 25);
  mock().actual_call("foo1").with_output_parameter("bar", &param);
  mock().actual_call("foo2").with_parameter("bar", 25);

  CHECK_EQUAL(2, retval);
  CHECK_EQUAL(2, param);
  mock().check_expectations();
}

TEST(MockParameter, outputAndInputParameter)
{
  int return_value = 5;
  int returned_value = 7;

  mock()
      .expect_one_call("foo")
      .with_parameter("bar", 10)
      .with_output_parameter_returning(
          "bar", &return_value, sizeof(return_value)
      );
  mock().actual_call("foo").with_parameter("bar", 10).with_output_parameter(
      "bar", &returned_value
  );

  CHECK_EQUAL(5, returned_value);
  mock().check_expectations();
}

TEST(MockParameter, outputParameterTraced)
{
  mock().tracing(true);

  int param = 1;
  mock().actual_call("someFunc").with_output_parameter("someParameter", &param);
  mock().check_expectations();
  STRCMP_CONTAINS(
      "Function name:someFunc someParameter:", mock().get_trace_output()
  );

  mock().check_expectations();
}

TEST(MockParameter, outputParameterThatIsIgnoredShouldNotFail)
{
  int param;
  mock().expect_one_call("function").ignore_other_parameters();
  mock().actual_call("function").with_output_parameter("parameterName", &param);

  mock().check_expectations();
}

TEST(MockParameter, outputParameterWithIgnoredParameters)
{
  int param = 1;
  int retval = 2;

  mock()
      .expect_one_call("foo")
      .with_output_parameter_returning("bar", &param, sizeof(param))
      .ignore_other_parameters();
  mock()
      .actual_call("foo")
      .with_output_parameter("bar", &retval)
      .with_parameter("other", 1);

  CHECK_EQUAL(param, retval);

  mock().check_expectations();
}

/*
 * This test checks that the proper output parameters are copied when multiple
 * calls to the same function are expected.
 */
TEST(MockParameter, properOutputParametersAreCopied)
{
  int expected_value1 = 1;
  int expected_value2 = 2;
  mock()
      .expect_one_call("foo")
      .with_output_parameter_returning(
          "param", &expected_value1, sizeof(expected_value1)
      )
      .ignore_other_parameters();
  mock().expect_one_call("foo").with_output_parameter_returning(
      "param", &expected_value2, sizeof(expected_value2)
  );

  int returned_value1 = 0;
  int returned_value2 = 0;
  mock().actual_call("foo").with_output_parameter("param", &returned_value1);
  mock()
      .actual_call("foo")
      .with_output_parameter("param", &returned_value2)
      .with_parameter("optional", 50);

  CHECK_EQUAL_TEXT(
      expected_value2, returned_value1, "Wrong output value in 1st call"
  );
  CHECK_EQUAL_TEXT(
      expected_value1, returned_value2, "Wrong output value in 2nd call"
  );

  mock().check_expectations();
}

TEST(MockParameter, ignoreOtherCallsIgnoresWithAllKindsOfParameters)
{
  mock().ignore_other_calls();
  mock()
      .actual_call("boo")
      .with_parameter("umm", true)
      .with_parameter("int", 1)
      .with_parameter("bar", 1U)
      .with_parameter("foo", 1L)
      .with_parameter("hey", 1UL)
      .with_parameter("ick", 1LL)
      .with_parameter("grr", 1ULL)
      .with_parameter("duh", 1.0)
      .with_parameter("str", "hello")
      .with_parameter("yoo", static_cast<const void*>(nullptr))
      .with_parameter("ptr", static_cast<void*>(nullptr))
      .with_parameter("func", static_cast<void (*)()>(nullptr))
      .with_parameter("mem", static_cast<const unsigned char*>(nullptr), 0)
      .with_parameter_of_type("hoo", "int", static_cast<const void*>(nullptr))
      .with_parameter_of_type(
          mu::tiny::String("hoo2"),
          mu::tiny::String("int"),
          static_cast<const void*>(nullptr)
      )
      .with_output_parameter("gah", static_cast<void*>(nullptr))
      .with_output_parameter_of_type("goo", "int", static_cast<void*>(nullptr));

  mock().check_expectations();
}

TEST(MockParameter, expectMultipleCallsWithParameters)
{
  int expected_int = -7;
  unsigned int expected_uint = 7;

  mock()
      .expect_n_calls(2, "boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", expected_int)
      .with_parameter("string", "string")
      .with_parameter("uint", expected_uint);
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", expected_int)
      .with_parameter("string", "string")
      .with_parameter("uint", expected_uint);
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", expected_int)
      .with_parameter("string", "string")
      .with_parameter("uint", expected_uint);

  mock().check_expectations();
}

TEST(MockParameter, twoOutputParametersSucceeds)
{
  int param1 = 1;
  int param2 = 3;
  const int retval1 = 2;
  const int retval2 = 4;

  mock()
      .expect_one_call("function")
      .with_output_parameter_returning("p1", &retval1, sizeof(retval1))
      .with_output_parameter_returning("p2", &retval2, sizeof(retval2));
  mock()
      .actual_call("function")
      .with_output_parameter("p1", &param1)
      .with_output_parameter("p2", &param2);

  CHECK_EQUAL(2, param1);
  CHECK_EQUAL(4, param2);
  mock().check_expectations();
}

TEST(MockParameter, expectMultipleMultipleCallsWithParameters)
{
  mock()
      .expect_n_calls(2, "boo")
      .with_parameter("double", 1.0)
      .ignore_other_parameters();
  mock()
      .expect_n_calls(2, "boo")
      .with_parameter("double", 1.0)
      .ignore_other_parameters();
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");
  mock()
      .actual_call("boo")
      .with_parameter("double", 1.0)
      .with_parameter("int", 1)
      .with_parameter("string", "string");

  mock().check_expectations();
}
