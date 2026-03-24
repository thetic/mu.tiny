#include "mutiny/mock.h"

#include "mutiny/mock/Failure.hpp"

#include "mutiny/test/Shell.hpp"

#include "mutiny/mock.hpp"

namespace {

class MockFailureReporterTestTerminatorForInCOnlyCode
  : public mu::tiny::test::Terminator
{
public:
  MockFailureReporterTestTerminatorForInCOnlyCode(bool crash_on_failure)
    : crash_on_failure_(crash_on_failure)
  {
  }

  void exit_current_test() const override
  {
    if (crash_on_failure_)
      mu::tiny::test::Shell::crash();

    mu::tiny::test::Shell::get_current_test_terminator_without_exceptions()
        .exit_current_test();
  }
  ~MockFailureReporterTestTerminatorForInCOnlyCode() override = default;

private:
  bool crash_on_failure_;
};

class MockFailureReporterForInCOnlyCode : public mu::tiny::mock::FailureReporter
{
public:
  void fail_test(mu::tiny::mock::Failure failure) override
  {
    if (!get_test_to_fail()->has_failed()) {
      get_test_to_fail()->add_failure(failure);
      {
        auto freed(static_cast<mu::tiny::mock::Failure&&>(failure));
      }
      exit_test();
    }
  }

  void exit_test() override
  {
    MockFailureReporterTestTerminatorForInCOnlyCode(crash_on_failure_)
        .exit_current_test();
  }
};

mu::tiny::mock::Support* current_mock_support = nullptr;
mu::tiny::mock::ExpectedCall* expected_call = nullptr;
mu::tiny::mock::ActualCall* current_actual_call = nullptr;
MockFailureReporterForInCOnlyCode failure_reporter_for_c;

class MockCFunctionComparatorNode : public mu::tiny::mock::NamedValueComparator
{
public:
  MockCFunctionComparatorNode(
      MockCFunctionComparatorNode* nx,
      MutinyMockTypeEqualFunction eq,
      MutinyMockTypeValueToStringFunction ts
  )
    : next(nx)
    , equal(eq)
    , to_string(ts)
  {
  }
  ~MockCFunctionComparatorNode() override = default;

  bool is_equal(const void* object1, const void* object2) override
  {
    return equal(object1, object2) != 0;
  }
  mu::tiny::test::String value_to_string(const void* object) override
  {
    return mu::tiny::test::String(to_string(object));
  }

  MockCFunctionComparatorNode* next;
  MutinyMockTypeEqualFunction equal;
  MutinyMockTypeValueToStringFunction to_string;
};

MockCFunctionComparatorNode* comparator_list = nullptr;

class MockCFunctionCopierNode : public mu::tiny::mock::NamedValueCopier
{
public:
  MockCFunctionCopierNode(
      MockCFunctionCopierNode* nx,
      MutinyMockTypeCopyFunction cp
  )
    : next(nx)
    , copier(cp)
  {
  }
  ~MockCFunctionCopierNode() override = default;

  void copy(void* dst, const void* src) override { copier(dst, src); }

  MockCFunctionCopierNode* next;
  MutinyMockTypeCopyFunction copier;
};

MockCFunctionCopierNode* copier_list = nullptr;

void install_comparator(
    const char* type_name,
    MutinyMockTypeEqualFunction is_equal,
    MutinyMockTypeValueToStringFunction value_to_string
)
{
  comparator_list = new MockCFunctionComparatorNode(
      comparator_list, is_equal, value_to_string
  );
  current_mock_support->install_comparator(type_name, *comparator_list);
}

void install_copier(const char* type_name, MutinyMockTypeCopyFunction copier)
{
  copier_list = new MockCFunctionCopierNode(copier_list, copier);
  current_mock_support->install_copier(type_name, *copier_list);
}

void remove_all_comparators_and_copiers()
{
  while (comparator_list) {
    MockCFunctionComparatorNode* next = comparator_list->next;
    delete comparator_list;
    comparator_list = next;
  }
  while (copier_list) {
    MockCFunctionCopierNode* next = copier_list->next;
    delete copier_list;
    copier_list = next;
  }
  current_mock_support->remove_all_comparators_and_copiers();
}

struct MutinyMockExpectedCall* expect_one_call(const char* name);
void expect_no_call(const char* name);
struct MutinyMockExpectedCall* expect_n_calls(
    const unsigned int number,
    const char* name
);

struct MutinyMockActualCall* actual_call(const char* name);
void disable();
void enable();
void ignore_other_calls();
void set_bool_data(const char* name, int value);
void set_int_data(const char* name, int value);
void set_unsigned_int_data(const char* name, unsigned int value);
void set_long_int_data(const char* name, long int value);
void set_unsigned_long_int_data(const char* name, unsigned long int value);
void set_double_data(const char* name, double value);
void set_string_data(const char* name, const char* value);
void set_pointer_data(const char* name, void* value);
void set_const_pointer_data(const char* name, const void* value);
void set_function_pointer_data(const char* name, void (*value)());
void set_data_object(const char* name, const char* type, void* value);
void set_data_const_object(
    const char* name,
    const char* type,
    const void* value
);
struct MutinyMockValue get_data(const char* name);
int has_return_value();

void check_expectations();
int expected_calls_left();
void clear();
void crash_on_failure(unsigned should_crash);

struct MutinyMockExpectedCall* with_bool_parameters(
    const char* name,
    int value
);
struct MutinyMockExpectedCall* with_int_parameters(const char* name, int value);
struct MutinyMockExpectedCall* with_unsigned_int_parameters(
    const char* name,
    unsigned int value
);
struct MutinyMockExpectedCall* with_long_int_parameters(
    const char* name,
    long int value
);
struct MutinyMockExpectedCall* with_unsigned_long_int_parameters(
    const char* name,
    unsigned long int value
);
struct MutinyMockExpectedCall* with_long_long_int_parameters(
    const char* name,
    long long value
);
struct MutinyMockExpectedCall* with_unsigned_long_long_int_parameters(
    const char* name,
    unsigned long long value
);
struct MutinyMockExpectedCall* with_double_parameters(
    const char* name,
    double value
);
struct MutinyMockExpectedCall* with_double_parameters_and_tolerance(
    const char* name,
    double value,
    double tolerance
);
struct MutinyMockExpectedCall* with_string_parameters(
    const char* name,
    const char* value
);
struct MutinyMockExpectedCall* with_pointer_parameters(
    const char* name,
    void* value
);
struct MutinyMockExpectedCall* with_const_pointer_parameters(
    const char* name,
    const void* value
);
struct MutinyMockExpectedCall* with_function_pointer_parameters(
    const char* name,
    void (*value)()
);
struct MutinyMockExpectedCall* with_memory_buffer_parameters(
    const char* name,
    const unsigned char* value,
    size_t size
);
struct MutinyMockExpectedCall* with_parameter_of_type(
    const char* type,
    const char* name,
    const void* value
);
struct MutinyMockExpectedCall* with_output_parameter_returning(
    const char* name,
    const void* value,
    size_t size
);
struct MutinyMockExpectedCall* with_output_parameter_of_type_returning(
    const char* type,
    const char* name,
    const void* value
);
struct MutinyMockExpectedCall* with_unmodified_output_parameter(
    const char* name
);
struct MutinyMockExpectedCall* ignore_other_parameters();
struct MutinyMockExpectedCall* and_return_bool_value(int value);
struct MutinyMockExpectedCall* and_return_int_value(int value);
struct MutinyMockExpectedCall* and_return_unsigned_int_value(
    unsigned int value
);
struct MutinyMockExpectedCall* and_return_long_int_value(long int value);
struct MutinyMockExpectedCall* and_return_unsigned_long_int_value(
    unsigned long int value
);
struct MutinyMockExpectedCall* and_return_long_long_int_value(long long value);
struct MutinyMockExpectedCall* and_return_unsigned_long_long_int_value(
    unsigned long long value
);
struct MutinyMockExpectedCall* and_return_double_value(double value);
struct MutinyMockExpectedCall* and_return_string_value(const char* value);
struct MutinyMockExpectedCall* and_return_pointer_value(void* value);
struct MutinyMockExpectedCall* and_return_const_pointer_value(
    const void* value
);
struct MutinyMockExpectedCall* and_return_function_pointer_value(
    void (*value)()
);

struct MutinyMockActualCall* with_actual_bool_parameters(
    const char* name,
    int value
);
struct MutinyMockActualCall* with_actual_int_parameters(
    const char* name,
    int value
);
struct MutinyMockActualCall* with_actual_unsigned_int_parameters(
    const char* name,
    unsigned int value
);
struct MutinyMockActualCall* with_actual_long_int_parameters(
    const char* name,
    long int value
);
struct MutinyMockActualCall* with_actual_unsigned_long_int_parameters(
    const char* name,
    unsigned long int value
);
struct MutinyMockActualCall* with_actual_long_long_int_parameters(
    const char* name,
    long long value
);
struct MutinyMockActualCall* with_actual_unsigned_long_long_int_parameters(
    const char* name,
    unsigned long long value
);
struct MutinyMockActualCall* with_actual_double_parameters(
    const char* name,
    double value
);
struct MutinyMockActualCall* with_actual_string_parameters(
    const char* name,
    const char* value
);
struct MutinyMockActualCall* with_actual_pointer_parameters(
    const char* name,
    void* value
);
struct MutinyMockActualCall* with_actual_const_pointer_parameters(
    const char* name,
    const void* value
);
struct MutinyMockActualCall* with_actual_function_pointer_parameters(
    const char* name,
    void (*value)()
);
struct MutinyMockActualCall* with_actual_memory_buffer_parameters(
    const char* name,
    const unsigned char* value,
    size_t size
);
struct MutinyMockActualCall* with_actual_parameter_of_type(
    const char* type,
    const char* name,
    const void* value
);
struct MutinyMockActualCall* with_actual_output_parameter(
    const char* name,
    void* value
);
struct MutinyMockActualCall* with_actual_output_parameter_of_type(
    const char* type,
    const char* name,
    void* value
);
struct MutinyMockValue return_value();
int bool_return_value();
int return_bool_value_or_default(int default_value);
int int_return_value();
int return_int_value_or_default(int default_value);
unsigned int unsigned_int_return_value();
unsigned int return_unsigned_int_value_or_default(unsigned int default_value);
long int long_int_return_value();
long int return_long_int_value_or_default(long int default_value);
unsigned long int unsigned_long_int_return_value();
unsigned long int return_unsigned_long_int_value_or_default(
    unsigned long int default_value
);
long long long_long_int_return_value();
long long return_long_long_int_value_or_default(long long default_value);
unsigned long long unsigned_long_long_int_return_value();
unsigned long long return_unsigned_long_long_int_value_or_default(
    unsigned long long default_value
);
const char* string_return_value();
const char* return_string_value_or_default(const char* default_value);
double double_return_value();
double return_double_value_or_default(double default_value);
void* pointer_return_value();
void* return_pointer_value_or_default(void* default_value);
const void* const_pointer_return_value();
const void* return_const_pointer_value_or_default(const void* default_value);
void (*function_pointer_return_value())();
void (*return_function_pointer_value_or_default(void (*default_value)()))();
struct MutinyMockExpectedCall g_expected_call = {
  with_bool_parameters,
  with_int_parameters,
  with_unsigned_int_parameters,
  with_long_int_parameters,
  with_unsigned_long_int_parameters,
  with_long_long_int_parameters,
  with_unsigned_long_long_int_parameters,
  with_double_parameters,
  with_double_parameters_and_tolerance,
  with_string_parameters,
  with_pointer_parameters,
  with_const_pointer_parameters,
  with_function_pointer_parameters,
  with_memory_buffer_parameters,
  with_parameter_of_type,
  with_output_parameter_returning,
  with_output_parameter_of_type_returning,
  with_unmodified_output_parameter,
  ignore_other_parameters,
  and_return_bool_value,
  and_return_unsigned_int_value,
  and_return_int_value,
  and_return_long_int_value,
  and_return_unsigned_long_int_value,
  and_return_long_long_int_value,
  and_return_unsigned_long_long_int_value,
  and_return_double_value,
  and_return_string_value,
  and_return_pointer_value,
  and_return_const_pointer_value,
  and_return_function_pointer_value,
};

struct MutinyMockActualCall g_actual_call = {
  with_actual_bool_parameters,
  with_actual_int_parameters,
  with_actual_unsigned_int_parameters,
  with_actual_long_int_parameters,
  with_actual_unsigned_long_int_parameters,
  with_actual_long_long_int_parameters,
  with_actual_unsigned_long_long_int_parameters,
  with_actual_double_parameters,
  with_actual_string_parameters,
  with_actual_pointer_parameters,
  with_actual_const_pointer_parameters,
  with_actual_function_pointer_parameters,
  with_actual_memory_buffer_parameters,
  with_actual_parameter_of_type,
  with_actual_output_parameter,
  with_actual_output_parameter_of_type,
  has_return_value,
  return_value,
  bool_return_value,
  return_bool_value_or_default,
  int_return_value,
  return_int_value_or_default,
  unsigned_int_return_value,
  return_unsigned_int_value_or_default,
  long_int_return_value,
  return_long_int_value_or_default,
  unsigned_long_int_return_value,
  return_unsigned_long_int_value_or_default,
  long_long_int_return_value,
  return_long_long_int_value_or_default,
  unsigned_long_long_int_return_value,
  return_unsigned_long_long_int_value_or_default,
  string_return_value,
  return_string_value_or_default,
  double_return_value,
  return_double_value_or_default,
  pointer_return_value,
  return_pointer_value_or_default,
  const_pointer_return_value,
  return_const_pointer_value_or_default,
  function_pointer_return_value,
  return_function_pointer_value_or_default
};

struct MutinyMockExpectedCall* with_bool_parameters(const char* name, int value)
{
  expected_call = &expected_call->with_parameter(name, (value != 0));
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_int_parameters(const char* name, int value)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_unsigned_int_parameters(
    const char* name,
    unsigned int value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_long_int_parameters(
    const char* name,
    long int value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_unsigned_long_int_parameters(
    const char* name,
    unsigned long int value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_long_long_int_parameters(
    const char* name,
    long long value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_unsigned_long_long_int_parameters(
    const char* name,
    unsigned long long value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_double_parameters(
    const char* name,
    double value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_double_parameters_and_tolerance(
    const char* name,
    double value,
    double tolerance
)
{
  expected_call = &expected_call->with_parameter(name, value, tolerance);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_string_parameters(
    const char* name,
    const char* value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_pointer_parameters(
    const char* name,
    void* value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_const_pointer_parameters(
    const char* name,
    const void* value
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_function_pointer_parameters(
    const char* name,
    void (*value)()
)
{
  expected_call = &expected_call->with_parameter(name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_memory_buffer_parameters(
    const char* name,
    const unsigned char* value,
    size_t size
)
{
  expected_call = &expected_call->with_parameter(name, value, size);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_parameter_of_type(
    const char* type,
    const char* name,
    const void* value
)
{
  expected_call = &expected_call->with_parameter_of_type(type, name, value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_output_parameter_returning(
    const char* name,
    const void* value,
    size_t size
)
{
  expected_call =
      &expected_call->with_output_parameter_returning(name, value, size);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_output_parameter_of_type_returning(
    const char* type,
    const char* name,
    const void* value
)
{
  expected_call = &expected_call->with_output_parameter_of_type_returning(
      type, name, value
  );
  return &g_expected_call;
}

struct MutinyMockExpectedCall* with_unmodified_output_parameter(
    const char* name
)
{
  expected_call = &expected_call->with_unmodified_output_parameter(name);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* ignore_other_parameters()
{
  expected_call = &expected_call->ignore_other_parameters();
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_bool_value(int value)
{
  expected_call = &expected_call->and_return_value(value != 0);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_unsigned_int_value(unsigned int value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_int_value(int value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_long_int_value(long int value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_unsigned_long_int_value(
    unsigned long int value
)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_long_long_int_value(long long value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_unsigned_long_long_int_value(
    unsigned long long value
)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_double_value(double value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_string_value(const char* value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_pointer_value(void* value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_const_pointer_value(const void* value)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockExpectedCall* and_return_function_pointer_value(
    void (*value)()
)
{
  expected_call = &expected_call->and_return_value(value);
  return &g_expected_call;
}

struct MutinyMockValue get_mock_value_c_from_named_value(
    const mu::tiny::mock::NamedValue& named_value
)
{
  using mu::tiny::test::strcmp;
  struct MutinyMockValue return_value;
  if (strcmp(named_value.get_type().c_str(), "bool") == 0) {
    return_value.type = mutiny_type_bool;
    return_value.value.bool_value = named_value.get_bool_value() ? 1 : 0;
  } else if (strcmp(named_value.get_type().c_str(), "int") == 0) {
    return_value.type = mutiny_type_integer;
    return_value.value.int_value = named_value.get_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "unsigned int") == 0) {
    return_value.type = mutiny_type_unsigned_integer;
    return_value.value.unsigned_int_value =
        named_value.get_unsigned_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "long int") == 0) {
    return_value.type = mutiny_type_long_integer;
    return_value.value.long_int_value = named_value.get_long_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "unsigned long int") == 0) {
    return_value.type = mutiny_type_unsigned_long_integer;
    return_value.value.unsigned_long_int_value =
        named_value.get_unsigned_long_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "long long int") == 0) {
    return_value.type = mutiny_type_long_long_integer;
    return_value.value.long_long_int_value =
        named_value.get_long_long_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "unsigned long long int") ==
             0) {
    return_value.type = mutiny_type_unsigned_long_long_integer;
    return_value.value.unsigned_long_long_int_value =
        named_value.get_unsigned_long_long_int_value();
  } else if (strcmp(named_value.get_type().c_str(), "double") == 0) {
    return_value.type = mutiny_type_double;
    return_value.value.double_value = named_value.get_double_value();
  } else if (strcmp(named_value.get_type().c_str(), "const char*") == 0) {
    return_value.type = mutiny_type_string;
    return_value.value.string_value = named_value.get_string_value();
  } else if (strcmp(named_value.get_type().c_str(), "void*") == 0) {
    return_value.type = mutiny_type_pointer;
    return_value.value.pointer_value = named_value.get_pointer_value();
  } else if (strcmp(named_value.get_type().c_str(), "const void*") == 0) {
    return_value.type = mutiny_type_const_pointer;
    return_value.value.const_pointer_value =
        named_value.get_const_pointer_value();
  } else if (strcmp(named_value.get_type().c_str(), "void (*)()") == 0) {
    return_value.type = mutiny_type_functionpointer;
    return_value.value.function_pointer_value =
        named_value.get_function_pointer_value();
  } else if (strcmp(named_value.get_type().c_str(), "const unsigned char*") ==
             0) {
    return_value.type = mutiny_type_memorybuffer;
    return_value.value.memory_buffer_value = named_value.get_memory_buffer();
  } else if (named_value.is_const_object()) {
    return_value.type = mutiny_type_const_object;
    return_value.value.const_object_value =
        named_value.get_const_object_pointer();
  } else {
    return_value.type = mutiny_type_object;
    return_value.value.object_value = named_value.get_object_pointer();
  }
  return return_value;
}

void strict_order()
{
  current_mock_support->strict_order();
}

struct MutinyMockExpectedCall* expect_one_call(const char* name)
{
  expected_call = &current_mock_support->expect_one_call(name);
  return &g_expected_call;
}

void expect_no_call(const char* name)
{
  current_mock_support->expect_no_call(name);
}

struct MutinyMockExpectedCall* expect_n_calls(
    const unsigned int number,
    const char* name
)
{
  expected_call = &current_mock_support->expect_n_calls(number, name);
  return &g_expected_call;
}

struct MutinyMockActualCall* actual_call(const char* name)
{
  current_actual_call = &current_mock_support->actual_call(name);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_bool_parameters(
    const char* name,
    int value
)
{
  current_actual_call =
      &current_actual_call->with_parameter(name, (value != 0));
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_int_parameters(
    const char* name,
    int value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_unsigned_int_parameters(
    const char* name,
    unsigned int value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_long_int_parameters(
    const char* name,
    long int value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_unsigned_long_int_parameters(
    const char* name,
    unsigned long int value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_long_long_int_parameters(
    const char* name,
    long long value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_unsigned_long_long_int_parameters(
    const char* name,
    unsigned long long value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_double_parameters(
    const char* name,
    double value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_string_parameters(
    const char* name,
    const char* value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_pointer_parameters(
    const char* name,
    void* value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_const_pointer_parameters(
    const char* name,
    const void* value
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_function_pointer_parameters(
    const char* name,
    void (*value)()
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_memory_buffer_parameters(
    const char* name,
    const unsigned char* value,
    size_t size
)
{
  current_actual_call = &current_actual_call->with_parameter(name, value, size);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_parameter_of_type(
    const char* type,
    const char* name,
    const void* value
)
{
  current_actual_call =
      &current_actual_call->with_parameter_of_type(type, name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_output_parameter(
    const char* name,
    void* value
)
{
  current_actual_call =
      &current_actual_call->with_output_parameter(name, value);
  return &g_actual_call;
}

struct MutinyMockActualCall* with_actual_output_parameter_of_type(
    const char* type,
    const char* name,
    void* value
)
{
  current_actual_call =
      &current_actual_call->with_output_parameter_of_type(type, name, value);
  return &g_actual_call;
}

struct MutinyMockValue return_value()
{
  return get_mock_value_c_from_named_value(current_actual_call->return_value());
}

int bool_return_value()
{
  return current_actual_call->return_bool_value() ? 1 : 0;
}

int return_bool_value_or_default(int default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return bool_return_value();
}

int int_return_value()
{
  return current_actual_call->return_int_value();
}

int return_int_value_or_default(int default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return int_return_value();
}

unsigned int unsigned_int_return_value()
{
  return current_actual_call->return_unsigned_int_value();
}

unsigned int return_unsigned_int_value_or_default(unsigned int default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return unsigned_int_return_value();
}

long int long_int_return_value()
{
  return current_actual_call->return_long_int_value();
}

long int return_long_int_value_or_default(long int default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return long_int_return_value();
}

unsigned long int unsigned_long_int_return_value()
{
  return current_actual_call->return_unsigned_long_int_value();
}

unsigned long int return_unsigned_long_int_value_or_default(
    unsigned long int default_value
)
{
  if (!has_return_value()) {
    return default_value;
  }
  return unsigned_long_int_return_value();
}

long long long_long_int_return_value()
{
  return current_actual_call->return_long_long_int_value();
}

long long return_long_long_int_value_or_default(long long default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return long_long_int_return_value();
}

unsigned long long unsigned_long_long_int_return_value()
{
  return current_actual_call->return_unsigned_long_long_int_value();
}

unsigned long long return_unsigned_long_long_int_value_or_default(
    unsigned long long default_value
)
{
  if (!has_return_value()) {
    return default_value;
  }
  return unsigned_long_long_int_return_value();
}

const char* string_return_value()
{
  return current_actual_call->return_string_value();
}

const char* return_string_value_or_default(const char* default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return string_return_value();
}

double double_return_value()
{
  return current_actual_call->return_double_value();
}

double return_double_value_or_default(double default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return double_return_value();
}

void* pointer_return_value()
{
  return current_actual_call->return_pointer_value();
}

void* return_pointer_value_or_default(void* default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return pointer_return_value();
}

const void* const_pointer_return_value()
{
  return current_actual_call->return_const_pointer_value();
}

const void* return_const_pointer_value_or_default(const void* default_value)
{
  if (!has_return_value()) {
    return default_value;
  }
  return const_pointer_return_value();
}

void (*function_pointer_return_value())()
{
  return current_actual_call->return_function_pointer_value();
}

void (*return_function_pointer_value_or_default(void (*default_value)()))()
{
  if (!has_return_value()) {
    return default_value;
  }
  return function_pointer_return_value();
}

void disable()
{
  current_mock_support->disable();
}

void enable()
{
  current_mock_support->enable();
}

void ignore_other_calls()
{
  current_mock_support->ignore_other_calls();
}

void set_bool_data(const char* name, int value)
{
  current_mock_support->set_data(name, (value != 0));
}

void set_int_data(const char* name, int value)
{
  current_mock_support->set_data(name, value);
}

void set_unsigned_int_data(const char* name, unsigned int value)
{
  current_mock_support->set_data(name, value);
}

void set_long_int_data(const char* name, long int value)
{
  current_mock_support->set_data(name, value);
}

void set_unsigned_long_int_data(const char* name, unsigned long int value)
{
  current_mock_support->set_data(name, value);
}

void set_double_data(const char* name, double value)
{
  current_mock_support->set_data(name, value);
}

void set_string_data(const char* name, const char* value)
{
  current_mock_support->set_data(name, value);
}

void set_pointer_data(const char* name, void* value)
{
  current_mock_support->set_data(name, value);
}

void set_const_pointer_data(const char* name, const void* value)
{
  current_mock_support->set_data(name, value);
}

void set_function_pointer_data(const char* name, void (*value)())
{
  current_mock_support->set_data(name, value);
}

void set_data_object(const char* name, const char* type, void* value)
{
  current_mock_support->set_data_object(name, type, value);
}

void set_data_const_object(
    const char* name,
    const char* type,
    const void* value
)
{
  current_mock_support->set_data_const_object(name, type, value);
}

struct MutinyMockValue get_data(const char* name)
{
  return get_mock_value_c_from_named_value(
      current_mock_support->get_data(name)
  );
}

int has_return_value()
{
  return current_mock_support->has_return_value();
}

void check_expectations()
{
  current_mock_support->check_expectations();
}

int expected_calls_left()
{
  return current_mock_support->expected_calls_left();
}

void clear()
{
  current_mock_support->clear();
}

void crash_on_failure(unsigned should_crash)
{
  current_mock_support->crash_on_failure(0 != should_crash);
}

struct MutinyMockSupport g_mock_support = {
  strict_order,
  expect_one_call,
  expect_no_call,
  expect_n_calls,
  actual_call,
  has_return_value,
  return_value,
  bool_return_value,
  return_bool_value_or_default,
  int_return_value,
  return_int_value_or_default,
  unsigned_int_return_value,
  return_unsigned_int_value_or_default,
  long_int_return_value,
  return_long_int_value_or_default,
  unsigned_long_int_return_value,
  return_unsigned_long_int_value_or_default,
  long_long_int_return_value,
  return_long_long_int_value_or_default,
  unsigned_long_long_int_return_value,
  return_unsigned_long_long_int_value_or_default,
  string_return_value,
  return_string_value_or_default,
  double_return_value,
  return_double_value_or_default,
  pointer_return_value,
  return_pointer_value_or_default,
  const_pointer_return_value,
  return_const_pointer_value_or_default,
  function_pointer_return_value,
  return_function_pointer_value_or_default,
  set_bool_data,
  set_int_data,
  set_unsigned_int_data,
  set_long_int_data,
  set_unsigned_long_int_data,
  set_string_data,
  set_double_data,
  set_pointer_data,
  set_const_pointer_data,
  set_function_pointer_data,
  set_data_object,
  set_data_const_object,
  get_data,
  disable,
  enable,
  ignore_other_calls,
  check_expectations,
  expected_calls_left,
  clear,
  crash_on_failure,
  install_comparator,
  install_copier,
  remove_all_comparators_and_copiers
};
} // namespace

struct MutinyMockSupport* mutiny_mock()
{
  current_mock_support = &mu::tiny::mock::mock("", &failure_reporter_for_c);
  return &g_mock_support;
}

struct MutinyMockSupport* mutiny_mock_scope(const char* scope)
{
  current_mock_support = &mu::tiny::mock::mock(scope, &failure_reporter_for_c);
  return &g_mock_support;
}
