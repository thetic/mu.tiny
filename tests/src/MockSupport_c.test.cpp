#include "MockSupport.h"

#include "mu/tiny/test/TestingFixture.hpp"

#include "mu/tiny/mock.h"
#include "mu/tiny/test.hpp"

namespace {
void dummy_function_for_mock_c_test() {}

void dummy_function_for_mock_c_test_two() {}

int type_name_is_equal(const void* object1, const void* object2)
{
  return static_cast<int>(object1 == object2);
}

const char* type_name_value_to_string(const void* /*object*/)
{
  return "valueToString";
}

void type_copy(void* dst, const void* src)
{
  *static_cast<int*>(dst) = *static_cast<const int*>(src);
}

bool destructor_was_called = false;

void failed_call_to_mock_c()
{
  struct SetBoolOnDestruct
  {
    bool& b;
    ~SetBoolOnDestruct() { b = true; }
  } set_one_destructor{ destructor_was_called };
  mutiny_mock()->actual_call("Not a call");
}

bool mutiny_has_crashed;

void crash_method()
{
  mutiny_has_crashed = true;
}

void failing_call_to_mock_c_with_parameter_of_type()
{
  mutiny_mock()->expect_one_call("bar")->with_parameter_of_type(
      "typeName", "name", reinterpret_cast<const void*>(1)
  );
  mutiny_mock()->actual_call("bar")->with_parameter_of_type(
      "typeName", "name", reinterpret_cast<const void*>(2)
  );
}

void call_to_mock_c_with_output_parameter_of_type()
{
  int value1 = 7;
  const int value2 = 9;
  mutiny_mock()
      ->expect_one_call("bar")
      ->with_output_parameter_of_type_returning("intType", "bla", &value2);
  mutiny_mock()->actual_call("bar")->with_output_parameter_of_type(
      "intType", "bla", &value1
  );
  CHECK_EQUAL(value1, value2);
}

void failing_call_to_mock_c_with_memory_buffer()
{
  unsigned char mem_buffer1[] = { 0x12, 0x15, 0xFF };
  unsigned char mem_buffer2[] = { 0x12, 0x05, 0xFF };
  mutiny_mock()->expect_one_call("bar")->with_memory_buffer_parameter(
      "name", mem_buffer1, sizeof(mem_buffer1)
  );
  mutiny_mock()->actual_call("bar")->with_memory_buffer_parameter(
      "name", mem_buffer2, sizeof(mem_buffer2)
  );
}

} // namespace

TEST_GROUP(MockSupport_c)
{
  void teardown() override { mutiny_mock()->clear(); }
};

TEST(MockSupport_c, OrderObserved)
{
  mutiny_mock()->strict_order();

  mutiny_mock()->expect_one_call("foo1");
  mutiny_mock()->expect_one_call("foo2");
  mutiny_mock()->actual_call("foo1");
  mutiny_mock()->actual_call("foo2");

  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, hasReturnValue)
{
  mutiny_mock()->expect_one_call("foo");
  CHECK(mutiny_mock()->actual_call("foo")->has_return_value() == 0);
  CHECK(mutiny_mock()->has_return_value() == 0);

  mutiny_mock()->expect_one_call("foo2")->and_return_int_value(1);
  CHECK(mutiny_mock()->actual_call("foo2")->has_return_value() != 0);
  CHECK(mutiny_mock()->has_return_value() != 0);
}

TEST(MockSupport_c, expectAndActualOneCall)
{
  mutiny_mock()->expect_one_call("boo");
  mutiny_mock()->actual_call("boo");
  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, expectAndActualThreeCalls)
{
  mutiny_mock()->expect_n_calls(3, "boo");
  mutiny_mock()->actual_call("boo");
  mutiny_mock()->actual_call("boo");
  mutiny_mock()->actual_call("boo");
  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, expectNoCall)
{
  mutiny_mock()->expect_no_call("foo");
  mutiny_mock()->expect_one_call("bar");
  mutiny_mock()->actual_call("bar");
  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, expectAndActualParameters)
{
  mutiny_mock()
      ->expect_one_call("boo")
      ->with_int_parameters("integer", 1)
      ->with_double_parameters("double", 1.0)
      ->with_string_parameters("string", "string")
      ->with_pointer_parameters("pointer", reinterpret_cast<void*>(1))
      ->with_function_pointer_parameters(
          "functionPointer", dummy_function_for_mock_c_test
      );
  mutiny_mock()
      ->actual_call("boo")
      ->with_int_parameters("integer", 1)
      ->with_double_parameters("double", 1.0)
      ->with_string_parameters("string", "string")
      ->with_pointer_parameters("pointer", reinterpret_cast<void*>(1))
      ->with_function_pointer_parameters(
          "functionPointer", dummy_function_for_mock_c_test
      );
}

TEST(MockSupport_c, expectAndActualParametersOnObject)
{
  mutiny_mock()->install_comparator(
      "typeName", type_name_is_equal, type_name_value_to_string
  );
  mutiny_mock()->expect_one_call("boo")->with_parameter_of_type(
      "typeName", "name", reinterpret_cast<const void*>(1)
  );
  mutiny_mock()->actual_call("boo")->with_parameter_of_type(
      "typeName", "name", reinterpret_cast<const void*>(1)
  );
  mutiny_mock()->check_expectations();
  mutiny_mock()->remove_all_comparators_and_copiers();
}

TEST(MockSupport_c, boolParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_bool_parameters("p", 1);
  mutiny_mock()->actual_call("foo")->with_bool_parameters("p", 1);
}

TEST(MockSupport_c, unsignedIntParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_unsigned_int_parameters("p", 1);
  mutiny_mock()->actual_call("foo")->with_unsigned_int_parameters("p", 1);
}

TEST(MockSupport_c, longIntParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_long_int_parameters("p", 1);
  mutiny_mock()->actual_call("foo")->with_long_int_parameters("p", 1);
}

TEST(MockSupport_c, unsignedLongIntParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_unsigned_long_int_parameters(
      "p", 1
  );
  mutiny_mock()->actual_call("foo")->with_unsigned_long_int_parameters("p", 1);
}

TEST(MockSupport_c, doubleParameterWithTolerance)
{
  mutiny_mock()->expect_one_call("foo")->with_double_parameters_and_tolerance(
      "p", 2.0, 0.2
  );
  mutiny_mock()->actual_call("foo")->with_double_parameters("p", 1.9);
}

TEST(MockSupport_c, longLongIntParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_long_long_int_parameters("p", 1);
  mutiny_mock()->actual_call("foo")->with_long_long_int_parameters("p", 1);
}

TEST(MockSupport_c, unsignedLongLongIntParameter)
{
  mutiny_mock()->expect_one_call("foo")->with_unsigned_long_long_int_parameters(
      "p", 1
  );
  mutiny_mock()->actual_call("foo")->with_unsigned_long_long_int_parameters(
      "p", 1
  );
}

TEST(MockSupport_c, memoryBufferParameter)
{
  const unsigned char mem_buffer[] = { 1, 2, 3 };
  mutiny_mock()->expect_one_call("foo")->with_memory_buffer_parameter(
      "out", mem_buffer, sizeof(mem_buffer)
  );
  mutiny_mock()->actual_call("foo")->with_memory_buffer_parameter(
      "out", mem_buffer, sizeof(mem_buffer)
  );
  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, outputParameters)
{
  int param = 1;
  const int retval = 2;
  mutiny_mock()->expect_one_call("foo")->with_output_parameter_returning(
      "out", &retval, sizeof(retval)
  );
  mutiny_mock()->actual_call("foo")->with_output_parameter("out", &param);
  mutiny_mock()->check_expectations();
  CHECK_EQUAL(2, param);
  CHECK_EQUAL(2, retval);
}

TEST(MockSupport_c, unmodifiedOutputParameter)
{
  int param = 1;
  mutiny_mock()->expect_one_call("foo")->with_unmodified_output_parameter(
      "out"
  );
  mutiny_mock()->actual_call("foo")->with_output_parameter("out", &param);
  mutiny_mock()->check_expectations();
  CHECK_EQUAL(1, param);
}

TEST(MockSupport_c, outputParameters_differentType)
{
  long param = 1;
  const long retval = 2;
  mutiny_mock()->expect_one_call("foo")->with_output_parameter_returning(
      "out", &retval, sizeof(retval)
  );
  mutiny_mock()->actual_call("foo")->with_output_parameter("out", &param);
  mutiny_mock()->check_expectations();
  CHECK_EQUAL(2, param);
  CHECK_EQUAL(2, retval);
}

TEST(MockSupport_c, outputParametersOfType)
{
  int param = 1;
  const int retval = 2;
  mutiny_mock()->install_copier("typeName", type_copy);
  mutiny_mock()
      ->expect_one_call("foo")
      ->with_output_parameter_of_type_returning("typeName", "out", &retval);
  mutiny_mock()->actual_call("foo")->with_output_parameter_of_type(
      "typeName", "out", &param
  );
  CHECK_EQUAL(2, param);
  CHECK_EQUAL(2, retval);
  mutiny_mock()->check_expectations();
  mutiny_mock()->remove_all_comparators_and_copiers();
}

TEST(MockSupport_c, capturedParameters)
{
  int captured = 1;
  int actual = 2;
  mutiny_mock()->expect_one_call("foo")->with_captured_parameter(
      "out", &captured, sizeof(captured)
  );
  mutiny_mock()->actual_call("foo")->with_captured_parameter("out", &actual);
  mutiny_mock()->check_expectations();
  CHECK_EQUAL(2, captured);
  CHECK_EQUAL(2, actual);
}

TEST(MockSupport_c, capturedParametersOfType)
{
  int captured = 1;
  int actual = 2;
  mutiny_mock()->install_copier("typeName", type_copy);
  mutiny_mock()->expect_one_call("foo")->with_captured_parameter_of_type(
      "typeName", "out", &captured
  );
  mutiny_mock()->actual_call("foo")->with_captured_parameter_of_type(
      "typeName", "out", &actual
  );
  CHECK_EQUAL(2, captured);
  CHECK_EQUAL(2, actual);
  mutiny_mock()->check_expectations();
  mutiny_mock()->remove_all_comparators_and_copiers();
}

TEST(MockSupport_c, ignoreOtherParameters)
{
  mutiny_mock()
      ->expect_one_call("foo")
      ->with_int_parameters("int", 1)
      ->ignore_other_parameters();
  mutiny_mock()
      ->actual_call("foo")
      ->with_int_parameters("int", 1)
      ->with_double_parameters("double", 0.01);
  mutiny_mock()->check_expectations();
}

TEST(MockSupport_c, returnBoolValue)
{
  int expected_value = 1;
  mutiny_mock()->expect_one_call("boo")->and_return_bool_value(expected_value);
  CHECK_EQUAL(
      expected_value, mutiny_mock()->actual_call("boo")->bool_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->bool_return_value());
  CHECK_EQUAL(mutiny_type_bool, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnBoolValueOrDefaultShouldIgnoreTheDefault)
{
  int default_value = 1;
  int expected_value = 0;
  mutiny_mock()->expect_one_call("foo")->and_return_bool_value(expected_value);
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_bool_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value, mutiny_mock()->return_bool_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnBoolValueOrDefaultShouldlUseTheDefaultValue)
{
  int default_value = 1;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_bool_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value, mutiny_mock()->return_bool_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnIntValue)
{
  int expected_value = -10;
  mutiny_mock()->expect_one_call("boo")->and_return_int_value(expected_value);
  CHECK_EQUAL(
      expected_value, mutiny_mock()->actual_call("boo")->int_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->int_return_value());
  CHECK_EQUAL(mutiny_type_integer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnIntValueOrDefaultShouldIgnoreTheDefault)
{
  int default_value = -10;
  int expected_value = default_value - 1;
  mutiny_mock()->expect_one_call("foo")->and_return_int_value(expected_value);
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value, mutiny_mock()->return_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnIntValueOrDefaultShouldlUseTheDefaultValue)
{
  int default_value = -10;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value, mutiny_mock()->return_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnUnsignedIntValue)
{
  unsigned int expected_value = 7;
  mutiny_mock()->expect_one_call("boo")->and_return_unsigned_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("boo")->unsigned_int_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->unsigned_int_return_value());
  CHECK_EQUAL(mutiny_type_unsigned_integer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnUnsignedIntValueOrDefaultShouldIgnoreTheDefault)
{
  unsigned int default_value = 10;
  unsigned int expected_value = default_value + 1;
  mutiny_mock()->expect_one_call("foo")->and_return_unsigned_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_unsigned_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_unsigned_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnUnsignedIntValueOrDefaultShouldlUseTheDefaultValue)
{
  unsigned int default_value = 10;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_unsigned_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_unsigned_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnLongIntValue)
{
  long int expected_value = -10L;
  mutiny_mock()->expect_one_call("boo")->and_return_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value, mutiny_mock()->actual_call("boo")->long_int_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->long_int_return_value());
  CHECK_EQUAL(mutiny_type_long_integer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnLongIntValueOrDefaultShouldIgnoreTheDefault)
{
  long int default_value = -10L;
  long int expected_value = default_value - 1L;
  mutiny_mock()->expect_one_call("foo")->and_return_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_long_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnLongIntValueOrDefaultShouldlUseTheDefaultValue)
{
  long int default_value = -10L;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_long_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnUnsignedLongIntValue)
{
  unsigned long int expected_value = 10;
  mutiny_mock()->expect_one_call("boo")->and_return_unsigned_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("boo")->unsigned_long_int_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->unsigned_long_int_return_value());
  CHECK_EQUAL(
      mutiny_type_unsigned_long_integer, mutiny_mock()->return_value().type
  );
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnUnsignedLongIntValueOrDefaultShouldIgnoreTheDefault)
{
  unsigned long int default_value = 10L;
  unsigned long int expected_value = default_value + 1L;
  mutiny_mock()->expect_one_call("foo")->and_return_unsigned_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()
          ->actual_call("foo")
          ->return_unsigned_long_int_value_or_default(default_value)
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_unsigned_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnUnsignedLongIntValueOrDefaultShouldlUseTheDefaultValue)
{
  unsigned long int default_value = 10L;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()
          ->actual_call("foo")
          ->return_unsigned_long_int_value_or_default(default_value)
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_unsigned_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnLongLongIntValue)
{
  long long int expected_value = -10L;
  mutiny_mock()->expect_one_call("boo")->and_return_long_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("boo")->long_long_int_return_value()
  );
  CHECK_EQUAL(expected_value, mutiny_mock()->long_long_int_return_value());
  CHECK_EQUAL(
      mutiny_type_long_long_integer, mutiny_mock()->return_value().type
  );
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnLongLongIntValueOrDefaultShouldIgnoreTheDefault)
{
  long long int default_value = -10L;
  long long int expected_value = default_value - 1L;
  mutiny_mock()->expect_one_call("foo")->and_return_long_long_int_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_long_long_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_long_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnLongLongIntValueOrDefaultShouldlUseTheDefaultValue)
{
  long long int default_value = -10L;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_long_long_int_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_long_long_int_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnUnsignedLongLongIntValue)
{
  unsigned long long int expected_value = 10;
  mutiny_mock()
      ->expect_one_call("boo")
      ->and_return_unsigned_long_long_int_value(expected_value);
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("boo")->unsigned_long_long_int_return_value()
  );
  CHECK_EQUAL(
      expected_value, mutiny_mock()->unsigned_long_long_int_return_value()
  );
  CHECK_EQUAL(
      mutiny_type_unsigned_long_long_integer, mutiny_mock()->return_value().type
  );
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnUnsignedLongLongIntValueOrDefaultShouldIgnoreTheDefault)
{
  unsigned long long int default_value = 10L;
  unsigned long long int expected_value = default_value + 1L;
  mutiny_mock()
      ->expect_one_call("foo")
      ->and_return_unsigned_long_long_int_value(expected_value);
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()
          ->actual_call("foo")
          ->return_unsigned_long_long_int_value_or_default(default_value)
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_unsigned_long_long_int_value_or_default(
          default_value
      )
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnUnsignedLongLongIntValueOrDefaultShouldlUseTheDefaultValue)
{
  unsigned long long int default_value = 10L;
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()
          ->actual_call("foo")
          ->return_unsigned_long_long_int_value_or_default(default_value)
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_unsigned_long_long_int_value_or_default(
          default_value
      )
  );
}

TEST(MockSupport_c, returnStringValue)
{
  mutiny_mock()->expect_one_call("boo")->and_return_string_value("hello world");
  STRCMP_EQUAL(
      "hello world", mutiny_mock()->actual_call("boo")->string_return_value()
  );
  STRCMP_EQUAL("hello world", mutiny_mock()->string_return_value());
  CHECK_EQUAL(mutiny_type_string, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnStringValueOrDefaultShouldIgnoreTheDefault)
{
  const char default_value[] = "bar";
  const char expected_value[] = "bla";
  mutiny_mock()->expect_one_call("foo")->and_return_string_value(
      expected_value
  );
  STRCMP_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_string_value_or_default(
          default_value
      )
  );
  STRCMP_EQUAL(
      expected_value,
      mutiny_mock()->return_string_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnStringValueOrDefaultShouldlUseTheDefaultValue)
{
  const char default_value[] = "bar";
  mutiny_mock()->expect_one_call("foo");
  STRCMP_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_string_value_or_default(
          default_value
      )
  );
  STRCMP_EQUAL(
      default_value,
      mutiny_mock()->return_string_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnDoubleValue)
{
  mutiny_mock()->expect_one_call("boo")->and_return_double_value(1.0);
  CHECK_APPROX(
      1.0, mutiny_mock()->actual_call("boo")->double_return_value(), 0.005
  );
  CHECK_APPROX(1.0, mutiny_mock()->double_return_value(), 0.005);
  CHECK_EQUAL(mutiny_type_double, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnDoubleValueOrDefaultShouldIgnoreTheDefault)
{
  double default_value = 2.2;
  double expected_value = default_value + 0.1;
  mutiny_mock()->expect_one_call("foo")->and_return_double_value(
      expected_value
  );
  CHECK_APPROX(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_double_value_or_default(
          default_value
      ),
      0.005
  );
  CHECK_APPROX(
      expected_value,
      mutiny_mock()->return_double_value_or_default(default_value),
      0.005
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnDoubleValueOrDefaultShouldlUseTheDefaultValue)
{
  double default_value = 2.2;
  mutiny_mock()->expect_one_call("foo");
  CHECK_APPROX(
      default_value,
      mutiny_mock()->actual_call("foo")->return_double_value_or_default(
          default_value
      ),
      0.005
  );
  CHECK_APPROX(
      default_value,
      mutiny_mock()->return_double_value_or_default(default_value),
      0.005
  );
}

TEST(MockSupport_c, returnPointerValue)
{
  mutiny_mock()->expect_one_call("boo")->and_return_pointer_value(
      reinterpret_cast<void*>(10)
  );
  CHECK_EQUAL(
      reinterpret_cast<void*>(10),
      mutiny_mock()->actual_call("boo")->pointer_return_value()
  );
  CHECK_EQUAL(
      reinterpret_cast<void*>(10), mutiny_mock()->pointer_return_value()
  );
  CHECK_EQUAL(mutiny_type_pointer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnPointerValueOrDefaultShouldIgnoreTheDefault)
{
  void* default_value = reinterpret_cast<void*>(10);
  void* expected_value = reinterpret_cast<void*>(27);
  mutiny_mock()->expect_one_call("foo")->and_return_pointer_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_pointer_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_pointer_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnPointerValueOrDefaultShouldlUseTheDefaultValue)
{
  void* default_value = reinterpret_cast<void*>(10);
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_pointer_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_pointer_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnConstPointerValue)
{
  mutiny_mock()->expect_one_call("boo")->and_return_const_pointer_value(
      reinterpret_cast<const void*>(10)
  );
  CHECK_EQUAL(
      reinterpret_cast<const void*>(10),
      mutiny_mock()->actual_call("boo")->const_pointer_return_value()
  );
  CHECK_EQUAL(
      reinterpret_cast<const void*>(10),
      mutiny_mock()->const_pointer_return_value()
  );
  CHECK_EQUAL(mutiny_type_const_pointer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnConstPointerValueOrDefaultShouldIgnoreTheDefault)
{
  const void* default_value = reinterpret_cast<void*>(10);
  const void* expected_value = reinterpret_cast<void*>(27);
  mutiny_mock()->expect_one_call("foo")->and_return_const_pointer_value(
      expected_value
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->actual_call("foo")->return_const_pointer_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      expected_value,
      mutiny_mock()->return_const_pointer_value_or_default(default_value)
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnConstPointerValueOrDefaultShouldlUseTheDefaultValue)
{
  const void* default_value = reinterpret_cast<void*>(10);
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->actual_call("foo")->return_const_pointer_value_or_default(
          default_value
      )
  );
  CHECK_EQUAL(
      default_value,
      mutiny_mock()->return_const_pointer_value_or_default(default_value)
  );
}

TEST(MockSupport_c, returnFunctionPointerValue)
{
  mutiny_mock()->expect_one_call("boo")->and_return_function_pointer_value(
      dummy_function_for_mock_c_test
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test,
      mutiny_mock()->actual_call("boo")->function_pointer_return_value()
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test,
      mutiny_mock()->function_pointer_return_value()
  );
  CHECK_EQUAL(mutiny_type_functionpointer, mutiny_mock()->return_value().type);
}

TEST(MockSupport_c, whenReturnValueIsGivenReturnFunctionPointerValueOrDefaultShouldIgnoreTheDefault)
{
  mutiny_mock()->expect_one_call("foo")->and_return_function_pointer_value(
      dummy_function_for_mock_c_test
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test,
      mutiny_mock()
          ->actual_call("foo")
          ->return_function_pointer_value_or_default(
              dummy_function_for_mock_c_test_two
          )
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test,
      mutiny_mock()->return_function_pointer_value_or_default(
          dummy_function_for_mock_c_test_two
      )
  );
}

TEST(MockSupport_c, whenNoReturnValueIsGivenReturnFunctionPointerValueOrDefaultShouldlUseTheDefaultValue)
{
  mutiny_mock()->expect_one_call("foo");
  CHECK_EQUAL(
      dummy_function_for_mock_c_test_two,
      mutiny_mock()
          ->actual_call("foo")
          ->return_function_pointer_value_or_default(
              dummy_function_for_mock_c_test_two
          )
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test_two,
      mutiny_mock()->return_function_pointer_value_or_default(
          dummy_function_for_mock_c_test_two
      )
  );
}

TEST(MockSupport_c, MockSupportWithScope)
{
  mutiny_mock_scope("scope")->expect_one_call("boo");
  CHECK_EQUAL(0, mutiny_mock_scope("other")->expected_calls_left());
  CHECK_EQUAL(1, mutiny_mock_scope("scope")->expected_calls_left());
  mutiny_mock_scope("scope")->actual_call("boo");
}

TEST(MockSupport_c, MockSupportSetBoolData)
{
  mutiny_mock()->set_bool_data("boolean", 1);
  CHECK_EQUAL(1, mutiny_mock()->get_data("boolean").value.bool_value);
}

TEST(MockSupport_c, MockSupportSetIntData)
{
  mutiny_mock()->set_int_data("integer", 10);
  CHECK_EQUAL(10, mutiny_mock()->get_data("integer").value.int_value);
}

TEST(MockSupport_c, MockSupportSetLongIntData)
{
  long int i = 100;
  mutiny_mock()->set_long_int_data("long integer", i);
  CHECK_EQUAL(i, mutiny_mock()->get_data("long integer").value.long_int_value);
}

TEST(MockSupport_c, MockSupportSetUnsignedLongIntData)
{
  unsigned long int i = 100;
  mutiny_mock()->set_unsigned_long_int_data("unsigned long integer", i);
  CHECK_EQUAL(
      i,
      mutiny_mock()
          ->get_data("unsigned long integer")
          .value.unsigned_long_int_value
  );
}

TEST(MockSupport_c, MockSupportSetLongLongIntData)
{
  long long i = 0x7FFFAAAAFFFFAAAALL;
  mutiny_mock()->set_long_long_int_data("long long integer", i);
  CHECK_EQUAL(
      i, mutiny_mock()->get_data("long long integer").value.long_long_int_value
  );
}

TEST(MockSupport_c, MockSupportSetUnsignedLongLongIntData)
{
  unsigned long long i = 0xFFFFAAAAFFFFAAAAULL;
  mutiny_mock()->set_unsigned_long_long_int_data(
      "unsigned long long integer", i
  );
  CHECK_EQUAL(
      i,
      mutiny_mock()
          ->get_data("unsigned long long integer")
          .value.unsigned_long_long_int_value
  );
}

TEST(MockSupport_c, MockSupportSetDoubleData)
{
  mutiny_mock()->set_double_data("double", 1.0);
  CHECK_APPROX(
      1.00, mutiny_mock()->get_data("double").value.double_value, 0.05
  );
}

TEST(MockSupport_c, MockSupportSetStringData)
{
  mutiny_mock()->set_string_data("string", "hello world");
  STRCMP_EQUAL(
      "hello world", mutiny_mock()->get_data("string").value.string_value
  );
}

TEST(MockSupport_c, MockSupportSetPointerData)
{
  mutiny_mock()->set_pointer_data("pointer", reinterpret_cast<void*>(1));
  CHECK_EQUAL(
      reinterpret_cast<void*>(1),
      mutiny_mock()->get_data("pointer").value.pointer_value
  );
}

TEST(MockSupport_c, MockSupportSetConstPointerData)
{
  mutiny_mock()->set_const_pointer_data(
      "constPointer", reinterpret_cast<const void*>(1)
  );
  CHECK_EQUAL(
      reinterpret_cast<const void*>(1),
      mutiny_mock()->get_data("constPointer").value.const_pointer_value
  );
}

TEST(MockSupport_c, MockSupportMemoryBufferData)
{
  mutiny_mock()->set_data_object(
      "name", "const unsigned char*", reinterpret_cast<void*>(0xDEAD)
  );
  CHECK_EQUAL(
      reinterpret_cast<void*>(0xDEAD),
      mutiny_mock()->get_data("name").value.memory_buffer_value
  );
  CHECK_EQUAL(mutiny_type_memorybuffer, mutiny_mock()->get_data("name").type);
}

TEST(MockSupport_c, MockSupportSetFunctionPointerData)
{
  mutiny_mock()->set_function_pointer_data(
      "functionPointer", dummy_function_for_mock_c_test
  );
  CHECK_EQUAL(
      dummy_function_for_mock_c_test,
      mutiny_mock()->get_data("functionPointer").value.function_pointer_value
  );
}

TEST(MockSupport_c, MockSupportSetDataObject)
{
  mutiny_mock()->set_data_object("name", "type", reinterpret_cast<void*>(1));
  CHECK_EQUAL(
      reinterpret_cast<void*>(1),
      mutiny_mock()->get_data("name").value.object_value
  );
  CHECK_EQUAL(mutiny_type_object, mutiny_mock()->get_data("name").type);
}

TEST(MockSupport_c, MockSupportSetDataConstObject)
{
  mutiny_mock()->set_data_const_object(
      "name", "type", reinterpret_cast<const void*>(5)
  );
  CHECK_EQUAL(
      reinterpret_cast<void*>(5),
      mutiny_mock()->get_data("name").value.const_object_value
  );
  CHECK_EQUAL(mutiny_type_const_object, mutiny_mock()->get_data("name").type);
}

TEST(MockSupport_c, WorksInCFile)
{
  mutiny_all_mock_support_c_calls();
}

// Silly wrapper because of a test that only fails in Visual C++ due to
// different destructor behaviors
#ifdef _MSC_VER
#define MSC_SWITCHED_TEST(testGroup, testName) SKIPPED_TEST(testGroup, testName)
#else
#define MSC_SWITCHED_TEST(testGroup, testName) TEST(testGroup, testName)
#endif

MSC_SWITCHED_TEST(MockSupport_c, NoExceptionsAreThrownWhenAMock_cCallFailed)
{
  mu::tiny::test::TestingFixture fixture;

  fixture.set_test_function(failed_call_to_mock_c);
  fixture.run_all_tests();

  CHECK_EQUAL(size_t{ 1 }, fixture.get_failure_count());
  // Odd behavior in Visual C++, destructor still gets called here
  CHECK(!destructor_was_called);
}

TEST(MockSupport_c, shouldCrashOnFailure)
{
  mutiny_has_crashed = false;
  mu::tiny::test::TestingFixture fixture;
  mu::tiny::test::Shell::set_crash_method(crash_method);
  mutiny_mock()->crash_on_failure(1U);
  fixture.set_test_function(failed_call_to_mock_c);

  fixture.run_all_tests();

  CHECK(mutiny_has_crashed);

  mu::tiny::test::Shell::reset_crash_method();
  mutiny_mock()->crash_on_failure(0U);
}

TEST(MockSupport_c, nextTestShouldNotCrashOnFailure)
{
  mutiny_mock()->crash_on_failure(0U);
  mutiny_has_crashed = false;
  mu::tiny::test::TestingFixture fixture;
  mu::tiny::test::Shell::set_crash_method(crash_method);
  fixture.set_test_function(failed_call_to_mock_c);

  fixture.run_all_tests();

  CHECK(!mutiny_has_crashed);

  mu::tiny::test::Shell::reset_crash_method();
}

TEST(MockSupport_c, FailWillNotCrashIfNotEnabled)
{
  mutiny_has_crashed = false;
  mu::tiny::test::TestingFixture fixture;
  mu::tiny::test::Shell::set_crash_method(crash_method);

  fixture.set_test_function(failed_call_to_mock_c);

  fixture.run_all_tests();

  CHECK(!mutiny_has_crashed);
  CHECK_EQUAL(size_t{ 1 }, fixture.get_failure_count());

  mu::tiny::test::Shell::reset_crash_method();
}

TEST(MockSupport_c, FailWillCrashIfEnabled)
{
  mutiny_has_crashed = false;
  mu::tiny::test::TestingFixture fixture;
  mu::tiny::test::Shell::set_crash_on_fail();
  mu::tiny::test::Shell::set_crash_method(crash_method);

  fixture.set_test_function(failed_call_to_mock_c);

  fixture.run_all_tests();

  CHECK(mutiny_has_crashed);
  CHECK_EQUAL(size_t{ 1 }, fixture.get_failure_count());

  mu::tiny::test::Shell::restore_default_test_terminator();
  mu::tiny::test::Shell::reset_crash_method();
}

TEST(MockSupport_c, failureWithParameterOfTypeCoversValueToString)
{
  mu::tiny::test::TestingFixture fixture;
  mutiny_mock()->install_comparator(
      "typeName", type_name_is_equal, type_name_value_to_string
  );
  fixture.set_test_function(failing_call_to_mock_c_with_parameter_of_type);
  fixture.run_all_tests();
  fixture.assert_print_contains("typeName name: <valueToString>");
  mutiny_mock()->remove_all_comparators_and_copiers();
}

TEST(MockSupport_c, successWithOutputParameterOfType)
{
  mu::tiny::test::TestingFixture fixture;
  mutiny_mock()->install_copier("intType", type_copy);
  fixture.set_test_function(call_to_mock_c_with_output_parameter_of_type);
  fixture.run_all_tests();
  CHECK_EQUAL(size_t{ 2 }, fixture.get_check_count());
  CHECK_EQUAL(size_t{ 0 }, fixture.get_failure_count());
  mutiny_mock()->remove_all_comparators_and_copiers();
}

TEST(MockSupport_c, expectOneMemBufferParameterAndValueFailsDueToContents)
{
  mu::tiny::test::TestingFixture fixture;
  fixture.set_test_function(failing_call_to_mock_c_with_memory_buffer);
  fixture.run_all_tests();
  fixture.assert_print_contains(
      "Unexpected parameter value to parameter \"name\" "
      "to function \"bar\": <Size = 3 | HexContents = 12 05 FF>"
  );
}

TEST(MockSupport_c, ignoreOtherCalls)
{
  mutiny_mock()->expect_one_call("foo");
  mutiny_mock()->ignore_other_calls();
  mutiny_mock()->actual_call("foo");
  mutiny_mock()->actual_call("bar");
  mutiny_mock()->check_expectations();
}
