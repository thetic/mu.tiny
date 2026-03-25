#include "mutiny/mock/CheckedExpectedCall.hpp"

namespace mu {
namespace tiny {
namespace mock {
using namespace mu::tiny::test;

String string_from(const NamedValue& parameter)
{
  return parameter.to_string();
}

void CheckedExpectedCall::set_name(const String& name)
{
  function_name_ = name;
}

String CheckedExpectedCall::get_name() const
{
  return function_name_;
}

CheckedExpectedCall::CheckedExpectedCall()
  : ignore_other_parameters_(false)
  , is_actual_call_match_finalized_(false)
  , initial_expected_call_order_(no_expected_call_order)
  , final_expected_call_order_(no_expected_call_order)
  , out_of_order_(false)
  , return_value_("")
  , object_ptr_(nullptr)
  , is_specific_object_expected_(false)
  , was_passed_to_object_(true)
  , actual_calls_(0)
  , expected_calls_(1)
{
  input_parameters_ = new NamedValueList();
  output_parameters_ = new NamedValueList();
}

CheckedExpectedCall::CheckedExpectedCall(unsigned int num_calls)
  : ignore_other_parameters_(false)
  , is_actual_call_match_finalized_(false)
  , initial_expected_call_order_(no_expected_call_order)
  , final_expected_call_order_(no_expected_call_order)
  , out_of_order_(false)
  , return_value_("")
  , object_ptr_(nullptr)
  , is_specific_object_expected_(false)
  , was_passed_to_object_(true)
  , actual_calls_(0)
  , expected_calls_(num_calls)
{
  input_parameters_ = new NamedValueList();
  output_parameters_ = new NamedValueList();
}

CheckedExpectedCall::~CheckedExpectedCall()
{
  input_parameters_->clear();
  delete input_parameters_;
  output_parameters_->clear();
  delete output_parameters_;
}

ExpectedCall& CheckedExpectedCall::with_name(const String& name)
{
  set_name(name);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_bool_parameter(
    const String& name,
    bool value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_unsigned_int_parameter(
    const String& name,
    unsigned int value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_int_parameter(
    const String& name,
    int value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_long_int_parameter(
    const String& name,
    long int value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_unsigned_long_int_parameter(
    const String& name,
    unsigned long int value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_long_long_int_parameter(
    const String& name,
    long long value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_unsigned_long_long_int_parameter(
    const String& name,
    unsigned long long value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_double_parameter(
    const String& name,
    double value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_double_parameter(
    const String& name,
    double value,
    double tolerance
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value, tolerance);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_string_parameter(
    const String& name,
    const char* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_pointer_parameter(
    const String& name,
    void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_const_pointer_parameter(
    const String& name,
    const void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_function_pointer_parameter(
    const String& name,
    void (*value)()
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_memory_buffer_parameter(
    const String& name,
    const unsigned char* value,
    size_t size
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_memory_buffer(value, size);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_parameter_of_type(
    const String& type,
    const String& name,
    const void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_const_object_pointer(type, value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_output_parameter_returning(
    const String& name,
    const void* value,
    size_t size
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  output_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  new_parameter->set_size(size);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_output_parameter_of_type_returning(
    const String& type,
    const String& name,
    const void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  output_parameters_->add(new_parameter);
  new_parameter->set_const_object_pointer(type, value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_unmodified_output_parameter(
    const String& name
)
{
  return with_output_parameter_returning(name, nullptr, 0);
}

String CheckedExpectedCall::get_input_parameter_type(const String& name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return (p) ? p->get_type() : mu::tiny::test::string_from("");
}

bool CheckedExpectedCall::has_input_parameter_with_name(const String& name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return p != nullptr;
}

bool CheckedExpectedCall::has_output_parameter_with_name(const String& name)
{
  NamedValue* p = output_parameters_->get_value_by_name(name);
  return p != nullptr;
}

NamedValue CheckedExpectedCall::get_input_parameter(const String& name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return (p) ? *p : NamedValue("");
}

NamedValue CheckedExpectedCall::get_output_parameter(const String& name)
{
  NamedValue* p = output_parameters_->get_value_by_name(name);
  return (p) ? *p : NamedValue("");
}

bool CheckedExpectedCall::are_parameters_matching_actual_call()
{
  NamedValueListNode* p;
  for (p = input_parameters_->begin(); p; p = p->next())
    if (!item(p)->is_matching_actual_call())
      return false;
  for (p = output_parameters_->begin(); p; p = p->next())
    if (!item(p)->is_matching_actual_call())
      return false;
  return true;
}

ExpectedCall& CheckedExpectedCall::ignore_other_parameters()
{
  ignore_other_parameters_ = true;
  return *this;
}

bool CheckedExpectedCall::is_fulfilled()
{
  return (actual_calls_ == expected_calls_);
}

bool CheckedExpectedCall::can_match_actual_calls()
{
  return (actual_calls_ < expected_calls_);
}

bool CheckedExpectedCall::is_matching_actual_call_and_finalized()
{
  return is_matching_actual_call() &&
         (!ignore_other_parameters_ || is_actual_call_match_finalized_);
}

bool CheckedExpectedCall::is_matching_actual_call()
{
  return are_parameters_matching_actual_call() && was_passed_to_object_;
}

void CheckedExpectedCall::call_was_made(unsigned int call_order)
{
  actual_calls_++;

  if ((initial_expected_call_order_ != no_expected_call_order) &&
      ((call_order < initial_expected_call_order_) ||
       (call_order > final_expected_call_order_))) {
    out_of_order_ = true;
  }

  reset_actual_call_matching_state();
}

void CheckedExpectedCall::finalize_actual_call_match()
{
  is_actual_call_match_finalized_ = true;
}

void CheckedExpectedCall::was_passed_to_object()
{
  was_passed_to_object_ = true;
}

void CheckedExpectedCall::reset_actual_call_matching_state()
{
  was_passed_to_object_ = !is_specific_object_expected_;
  is_actual_call_match_finalized_ = false;

  NamedValueListNode* p;

  for (p = input_parameters_->begin(); p; p = p->next())
    item(p)->set_matches_actual_call(false);
  for (p = output_parameters_->begin(); p; p = p->next())
    item(p)->set_matches_actual_call(false);
}

void CheckedExpectedCall::input_parameter_was_passed(const String& name)
{
  for (NamedValueListNode* p = input_parameters_->begin(); p; p = p->next()) {
    if (p->get_name() == name)
      item(p)->set_matches_actual_call(true);
  }
}

void CheckedExpectedCall::output_parameter_was_passed(const String& name)
{
  for (NamedValueListNode* p = output_parameters_->begin(); p; p = p->next()) {
    if (p->get_name() == name)
      item(p)->set_matches_actual_call(true);
  }
}

String CheckedExpectedCall::get_input_parameter_value_string(const String& name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return (p) ? string_from(*p) : mu::tiny::test::string_from("failed");
}

bool CheckedExpectedCall::has_input_parameter(const NamedValue& parameter)
{
  NamedValue* p = input_parameters_->get_value_by_name(parameter.get_name());
  return (p) ? p->equals(parameter) : ignore_other_parameters_;
}

bool CheckedExpectedCall::has_output_parameter(const NamedValue& parameter)
{
  NamedValue* p = output_parameters_->get_value_by_name(parameter.get_name());
  return (p) ? p->compatible_for_copying(parameter) : ignore_other_parameters_;
}

String CheckedExpectedCall::call_to_string()
{
  String str;
  if (is_specific_object_expected_)
    str = string_from_format("(object address: %p)::", object_ptr_);

  str += get_name();
  str += MUTINY_STR(" -> ");
  if (initial_expected_call_order_ != no_expected_call_order) {
    if (initial_expected_call_order_ == final_expected_call_order_) {
      str += string_from_format(
          "expected call order: <%u> -> ", initial_expected_call_order_
      );
    } else {
      str += string_from_format(
          "expected calls order: <%u..%u> -> ",
          initial_expected_call_order_,
          final_expected_call_order_
      );
    }
  }

  if (input_parameters_->begin() == nullptr &&
      output_parameters_->begin() == nullptr) {
    str += (ignore_other_parameters_) ? MUTINY_STR("all parameters ignored")
                                      : MUTINY_STR("no parameters");
  } else {
    NamedValueListNode* p;

    for (p = input_parameters_->begin(); p; p = p->next()) {
      str += string_from_format(
          "%s %s: <%s>",
          p->get_type().c_str(),
          p->get_name().c_str(),
          get_input_parameter_value_string(p->get_name()).c_str()
      );
      if (p->next())
        str += MUTINY_STR(", ");
    }

    if (input_parameters_->begin() && output_parameters_->begin()) {
      str += MUTINY_STR(", ");
    }

    for (p = output_parameters_->begin(); p; p = p->next()) {
      str += string_from_format(
          "%s %s: <output>", p->get_type().c_str(), p->get_name().c_str()
      );
      if (p->next())
        str += MUTINY_STR(", ");
    }

    if (ignore_other_parameters_)
      str += MUTINY_STR(", other parameters are ignored");
  }

  str += string_from_format(
      " (expected %u call%s, called %u time%s)",
      expected_calls_,
      (expected_calls_ == 1) ? "" : "s",
      actual_calls_,
      (actual_calls_ == 1) ? "" : "s"
  );

  return str;
}

String CheckedExpectedCall::missing_parameters_to_string()
{
  String str;
  NamedValueListNode* p;

  for (p = input_parameters_->begin(); p; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      if (str != "")
        str += MUTINY_STR(", ");
      str += string_from_format(
          "%s %s", p->get_type().c_str(), p->get_name().c_str()
      );
    }
  }
  for (p = output_parameters_->begin(); p; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      if (str != "")
        str += MUTINY_STR(", ");
      str += string_from_format(
          "%s %s", p->get_type().c_str(), p->get_name().c_str()
      );
    }
  }
  return str;
}

bool CheckedExpectedCall::relates_to(const String& function_name)
{
  return function_name == get_name();
}

bool CheckedExpectedCall::relates_to_object(const void* object_ptr) const
{
  return (!is_specific_object_expected_) || (object_ptr_ == object_ptr);
}

CheckedExpectedCall::MockExpectedFunctionParameter* CheckedExpectedCall::item(
    NamedValueListNode* node
)
{
  return static_cast<MockExpectedFunctionParameter*>(node->item());
}

CheckedExpectedCall::MockExpectedFunctionParameter::
    MockExpectedFunctionParameter(const String& name)
  : NamedValue(name)

{
}

void CheckedExpectedCall::MockExpectedFunctionParameter::
    set_matches_actual_call(bool b)
{
  matches_actual_call_ = b;
}

bool CheckedExpectedCall::MockExpectedFunctionParameter::
    is_matching_actual_call() const
{
  return matches_actual_call_;
}

ExpectedCall& CheckedExpectedCall::and_return_value(bool value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(unsigned int value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(int value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(long int value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(unsigned long int value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(long long value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(unsigned long long value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(const char* value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(double value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(void* value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(const void* value)
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::and_return_value(void (*value)())
{
  return_value_.set_name("returnValue");
  return_value_.set_value(value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::on_object(void* object_ptr)
{
  is_specific_object_expected_ = true;
  was_passed_to_object_ = false;
  object_ptr_ = object_ptr;
  return *this;
}

NamedValue CheckedExpectedCall::return_value()
{
  return return_value_;
}

ExpectedCall& CheckedExpectedCall::with_call_order(
    unsigned int initial_call_order,
    unsigned int final_call_order
)
{
  initial_expected_call_order_ = initial_call_order;
  final_expected_call_order_ = final_call_order;
  return *this;
}

bool CheckedExpectedCall::is_out_of_order() const
{
  return out_of_order_;
}

unsigned int CheckedExpectedCall::get_actual_calls_fulfilled() const
{
  return actual_calls_;
}

} // namespace mock
} // namespace tiny
} // namespace mu
