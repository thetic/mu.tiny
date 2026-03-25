#include "mutiny/mock.hpp"

#include "mutiny/mock/ActualCallTrace.hpp"
#include "mutiny/mock/CheckedActualCall.hpp"
#include "mutiny/mock/CheckedExpectedCall.hpp"
#include "mutiny/mock/ExpectedCall.hpp"
#include "mutiny/mock/ExpectedCallsList.hpp"
#include "mutiny/mock/Failure.hpp"
#include "mutiny/mock/IgnoredActualCall.hpp"
#include "mutiny/mock/IgnoredExpectedCall.hpp"
#include "mutiny/mock/NamedValueComparatorsAndCopiersRepository.hpp"
#include "mutiny/mock/NamedValueList.hpp"

#include "mutiny/test/Shell.hpp"

#define MOCK_SUPPORT_SCOPE_PREFIX "!!!$$$MockingSupportScope$$$!!!"

namespace mu {
namespace tiny {
namespace mock {

class Support::Impl
{
  friend class Support;

  FailureReporter default_reporter_;
  FailureReporter* active_reporter_{ nullptr };
  FailureReporter* standard_reporter_;
  ExpectedCallsList expectations_;
  CheckedActualCall* last_actual_function_call_{ nullptr };
  NamedValueComparatorsAndCopiersRepository comparators_and_copiers_repository_;
  NamedValueList data_;

  Impl()
    : standard_reporter_(&default_reporter_)

  {
  }
};

namespace {
Support global_mock;
} // namespace

Support& mock(
    const test::String& mock_name,
    FailureReporter* failure_reporter_for_this_call
)
{
  Support& mock_support = (mock_name != "")
                              ? *global_mock.get_mock_support_scope(mock_name)
                              : global_mock;
  mock_support.set_active_reporter(failure_reporter_for_this_call);
  mock_support.set_default_comparators_and_copiers_repository();
  return mock_support;
}

Support::Support(const test::String& mock_name)
  : impl_(new Impl())
  , mock_name_(mock_name)

{
}

Support::~Support()
{
  delete impl_;
}

void Support::crash_on_failure(bool should_crash)
{
  impl_->active_reporter_->crash_on_failure(should_crash);
}

void Support::set_mock_failure_standard_reporter(FailureReporter* reporter)
{
  impl_->standard_reporter_ =
      (reporter != nullptr) ? reporter : &impl_->default_reporter_;

  if (impl_->last_actual_function_call_)
    impl_->last_actual_function_call_->set_mock_failure_reporter(
        impl_->standard_reporter_
    );

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->set_mock_failure_standard_reporter(
          impl_->standard_reporter_
      );
}

void Support::set_active_reporter(FailureReporter* reporter)
{
  impl_->active_reporter_ = (reporter) ? reporter : impl_->standard_reporter_;
}

void Support::set_default_comparators_and_copiers_repository()
{
  NamedValue::set_default_comparators_and_copiers_repository(
      &impl_->comparators_and_copiers_repository_
  );
}

void Support::install_comparator(
    const test::String& type_name,
    NamedValueComparator& comparator
)
{
  impl_->comparators_and_copiers_repository_.install_comparator(
      type_name, comparator
  );

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->install_comparator(type_name, comparator);
}

void Support::install_copier(
    const test::String& type_name,
    NamedValueCopier& copier
)
{
  impl_->comparators_and_copiers_repository_.install_copier(type_name, copier);

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->install_copier(type_name, copier);
}

void Support::install_comparators_and_copiers(
    const NamedValueComparatorsAndCopiersRepository& repository
)
{
  impl_->comparators_and_copiers_repository_.install_comparators_and_copiers(
      repository
  );

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->install_comparators_and_copiers(repository);
}

void Support::remove_all_comparators_and_copiers()
{
  impl_->comparators_and_copiers_repository_.clear();
  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->remove_all_comparators_and_copiers();
}

void Support::clear()
{
  delete impl_->last_actual_function_call_;
  impl_->last_actual_function_call_ = nullptr;

  tracing_ = false;
  ActualCallTrace::clear_instance();

  impl_->expectations_.delete_all_expectations_and_clear_list();
  ignore_other_calls_ = false;
  enabled_ = true;
  actual_call_order_ = 0;
  expected_call_order_ = 0;
  strict_ordering_ = false;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next()) {
    Support* support = get_mock_support(p);
    if (support) {
      support->clear();
      delete support;
    }
  }
  impl_->data_.clear();
}

void Support::strict_order()
{
  strict_ordering_ = true;
}

test::String Support::append_scope_to_name(const test::String& function_name)
{
  if (mock_name_.empty())
    return function_name;
  return mock_name_ + MUTINY_STR("::") + function_name;
}

ExpectedCall& Support::expect_one_call(const test::String& function_name)
{
  return expect_n_calls(1, function_name);
}

void Support::expect_no_call(const test::String& function_name)
{
  expect_n_calls(0, function_name);
}

ExpectedCall& Support::expect_n_calls(
    unsigned int amount,
    const test::String& function_name
)
{
  if (!enabled_)
    return IgnoredExpectedCall::instance();

  count_check();

  auto* call = new CheckedExpectedCall(amount);
  call->with_name(append_scope_to_name(function_name));
  if (strict_ordering_) {
    call->with_call_order(
        expected_call_order_ + 1, expected_call_order_ + amount
    );
    expected_call_order_ += amount;
  }
  impl_->expectations_.add_expected_call(call);
  return *call;
}

CheckedActualCall* Support::create_actual_call()
{
  impl_->last_actual_function_call_ = new CheckedActualCall(
      ++actual_call_order_, impl_->active_reporter_, impl_->expectations_
  );
  return impl_->last_actual_function_call_;
}

bool Support::call_is_ignored(const test::String& function_name)
{
  return ignore_other_calls_ &&
         !impl_->expectations_.has_expectation_with_name(function_name);
}

ActualCall& Support::actual_call(const char* function_name)
{
  test::String scope_function_name = append_scope_to_name(function_name);

  if (impl_->last_actual_function_call_) {
    impl_->last_actual_function_call_->check_expectations();
    delete impl_->last_actual_function_call_;
    impl_->last_actual_function_call_ = nullptr;
  }

  if (!enabled_)
    return IgnoredActualCall::instance();
  if (tracing_)
    return ActualCallTrace::instance().with_name(scope_function_name);

  if (call_is_ignored(scope_function_name)) {
    return IgnoredActualCall::instance();
  }

  CheckedActualCall* call = create_actual_call();
  call->set_name_and_check(static_cast<test::String&&>(scope_function_name));
  return *call;
}

void Support::ignore_other_calls()
{
  ignore_other_calls_ = true;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->ignore_other_calls();
}

void Support::disable()
{
  enabled_ = false;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->disable();
}

void Support::enable()
{
  enabled_ = true;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->enable();
}

void Support::tracing(bool enabled)
{
  tracing_ = enabled;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      get_mock_support(p)->tracing(enabled);
}

const char* Support::get_trace_output()
{
  return ActualCallTrace::instance().get_trace_output();
}

bool Support::expected_calls_left()
{
  check_expectations_of_last_actual_call();
  int calls_left = impl_->expectations_.has_unfulfilled_expectations();

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      calls_left += get_mock_support(p)->expected_calls_left();

  return calls_left != 0;
}

bool Support::was_last_actual_call_fulfilled()
{
  if (impl_->last_actual_function_call_ &&
      !impl_->last_actual_function_call_->is_fulfilled())
    return false;

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p) &&
        !get_mock_support(p)->was_last_actual_call_fulfilled())
      return false;

  return true;
}

void Support::fail_test_with_expected_calls_not_fulfilled()
{
  ExpectedCallsList expectations_list;
  expectations_list.add_expectations(impl_->expectations_);

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      expectations_list.add_expectations(
          get_mock_support(p)->impl_->expectations_
      );

  ExpectedCallsDidntHappenFailure failure(
      impl_->active_reporter_->get_test_to_fail(), expectations_list
  );
  fail_test(failure);
}

void Support::fail_test_with_out_of_order_calls()
{
  ExpectedCallsList expectations_list;
  expectations_list.add_expectations(impl_->expectations_);

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p))
      expectations_list.add_expectations(
          get_mock_support(p)->impl_->expectations_
      );

  CallOrderFailure failure(
      impl_->active_reporter_->get_test_to_fail(), expectations_list
  );
  fail_test(failure);
}

void Support::fail_test(Failure& failure)
{
  clear();
  impl_->active_reporter_->fail_test(static_cast<Failure&&>(failure));
}

void Support::count_check()
{
  mu::tiny::test::Shell::get_current()->count_check();
}

void Support::check_expectations_of_last_actual_call()
{
  if (impl_->last_actual_function_call_)
    impl_->last_actual_function_call_->check_expectations();

  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p) &&
        get_mock_support(p)->impl_->last_actual_function_call_)
      get_mock_support(p)
          ->impl_->last_actual_function_call_->check_expectations();
}

bool Support::has_calls_out_of_order()
{
  if (impl_->expectations_.has_calls_out_of_order()) {
    return true;
  }
  for (NamedValueListNode* p = impl_->data_.begin(); p; p = p->next())
    if (get_mock_support(p) && get_mock_support(p)->has_calls_out_of_order()) {
      return true;
    }
  return false;
}

void Support::check_expectations()
{
  check_expectations_of_last_actual_call();

  if (was_last_actual_call_fulfilled() && expected_calls_left())
    fail_test_with_expected_calls_not_fulfilled();

  if (has_calls_out_of_order())
    fail_test_with_out_of_order_calls();
}

bool Support::has_data(const test::String& name)
{
  return impl_->data_.get_value_by_name(name) != nullptr;
}

NamedValue* Support::retrieve_data_from_store(const test::String& name)
{
  NamedValue* new_data = impl_->data_.get_value_by_name(name);
  if (new_data == nullptr) {
    new_data = new NamedValue(name);
    impl_->data_.add(new_data);
  }
  return new_data;
}

void Support::set_data(const test::String& name, bool value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, unsigned int value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, int value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, long int value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, unsigned long int value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, const char* value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, double value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, void* value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, const void* value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data(const test::String& name, FunctionPointerValue value)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_value(value);
}

void Support::set_data_object(
    const test::String& name,
    const test::String& type,
    void* value
)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_object_pointer(type, value);
}

void Support::set_data_const_object(
    const test::String& name,
    const test::String& type,
    const void* value
)
{
  NamedValue* new_data = retrieve_data_from_store(name);
  new_data->set_const_object_pointer(type, value);
}

NamedValue Support::get_data(const test::String& name)
{
  NamedValue* value = impl_->data_.get_value_by_name(name);
  if (value == nullptr)
    return NamedValue("");
  return *value;
}

Support* Support::clone(const test::String& mock_name)
{
  auto* new_mock = new Support(mock_name);
  new_mock->set_mock_failure_standard_reporter(impl_->standard_reporter_);
  if (ignore_other_calls_)
    new_mock->ignore_other_calls();

  if (!enabled_)
    new_mock->disable();

  if (strict_ordering_)
    new_mock->strict_order();

  new_mock->tracing(tracing_);
  new_mock->install_comparators_and_copiers(
      impl_->comparators_and_copiers_repository_
  );
  return new_mock;
}

Support* Support::get_mock_support_scope(const test::String& name)
{
  test::String mocking_support_name = MOCK_SUPPORT_SCOPE_PREFIX;
  mocking_support_name += name;

  if (has_data(mocking_support_name)) {
    STRCMP_EQUAL("Support", get_data(mocking_support_name).get_type().c_str());
    return static_cast<Support*>(
        get_data(mocking_support_name).get_object_pointer()
    );
  }

  Support* new_mock = clone(name);

  set_data_object(mocking_support_name, "Support", new_mock);
  return new_mock;
}

Support* Support::get_mock_support(NamedValueListNode* node)
{
  if (node->get_type() == "Support" &&
      test::string_contains(node->get_name(), MOCK_SUPPORT_SCOPE_PREFIX))
    return static_cast<Support*>(node->item()->get_object_pointer());
  return nullptr;
}

NamedValue Support::return_value()
{
  if (impl_->last_actual_function_call_)
    return impl_->last_actual_function_call_->return_value();
  return NamedValue("");
}

bool Support::bool_return_value()
{
  return return_value().get_bool_value();
}

unsigned int Support::unsigned_int_return_value()
{
  return return_value().get_unsigned_int_value();
}

int Support::int_return_value()
{
  return return_value().get_int_value();
}

const char* Support::return_string_value_or_default(const char* default_value)
{
  if (has_return_value()) {
    return string_return_value();
  }
  return default_value;
}

double Support::return_double_value_or_default(double default_value)
{
  if (has_return_value()) {
    return double_return_value();
  }
  return default_value;
}

long int Support::return_long_int_value_or_default(long int default_value)
{
  if (has_return_value()) {
    return long_int_return_value();
  }
  return default_value;
}

bool Support::return_bool_value_or_default(bool default_value)
{
  if (has_return_value()) {
    return bool_return_value();
  }
  return default_value;
}

int Support::return_int_value_or_default(int default_value)
{
  if (has_return_value()) {
    return int_return_value();
  }
  return default_value;
}

unsigned int Support::return_unsigned_int_value_or_default(
    unsigned int default_value
)
{
  if (has_return_value()) {
    return unsigned_int_return_value();
  }
  return default_value;
}

unsigned long int Support::return_unsigned_long_int_value_or_default(
    unsigned long int default_value
)
{
  if (has_return_value()) {
    return unsigned_long_int_return_value();
  }
  return default_value;
}

long int Support::long_int_return_value()
{
  return return_value().get_long_int_value();
}

unsigned long int Support::unsigned_long_int_return_value()
{
  return return_value().get_unsigned_long_int_value();
}

long long Support::long_long_int_return_value()
{
  return return_value().get_long_long_int_value();
}

unsigned long long Support::unsigned_long_long_int_return_value()
{
  return return_value().get_unsigned_long_long_int_value();
}

long long Support::return_long_long_int_value_or_default(
    long long default_value
)
{
  if (has_return_value()) {
    return long_long_int_return_value();
  }
  return default_value;
}

unsigned long long Support::return_unsigned_long_long_int_value_or_default(
    unsigned long long default_value
)
{
  if (has_return_value()) {
    return unsigned_long_long_int_return_value();
  }
  return default_value;
}

const char* Support::string_return_value()
{
  return return_value().get_string_value();
}

double Support::double_return_value()
{
  return return_value().get_double_value();
}

void* Support::return_pointer_value_or_default(void* default_value)
{
  if (has_return_value()) {
    return pointer_return_value();
  }
  return default_value;
}

const void* Support::return_const_pointer_value_or_default(
    const void* default_value
)
{
  if (has_return_value()) {
    return const_pointer_return_value();
  }
  return default_value;
}

Support::FunctionPointerValue Support::return_function_pointer_value_or_default(
    FunctionPointerValue default_value
)
{
  if (has_return_value()) {
    return function_pointer_return_value();
  }
  return default_value;
}

void* Support::pointer_return_value()
{
  return return_value().get_pointer_value();
}

const void* Support::const_pointer_return_value()
{
  return return_value().get_const_pointer_value();
}

Support::FunctionPointerValue Support::function_pointer_return_value()
{
  return return_value().get_function_pointer_value();
}

bool Support::has_return_value()
{
  if (impl_->last_actual_function_call_)
    return impl_->last_actual_function_call_->has_return_value();
  return false;
}

} // namespace mock
} // namespace tiny
} // namespace mu
