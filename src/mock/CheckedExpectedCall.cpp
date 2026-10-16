#include "mu/tiny/mock/CheckedExpectedCall.hpp"

#include "mu/tiny/String.hpp"

namespace mu {
namespace tiny {

String string_from(const mock::NamedValue& parameter)
{
  return parameter.to_string();
}

namespace mock {

void CheckedExpectedCall::set_name(StringView name)
{
  function_name_ = String(name.data(), name.size());
}

String CheckedExpectedCall::get_name() const
{
  return function_name_;
}

CheckedExpectedCall::CheckedExpectedCall(unsigned int num_calls)
  : expected_calls_(num_calls)
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

ExpectedCall& CheckedExpectedCall::with_name(StringView name)
{
  set_name(name);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_typed_parameter(NamedValue parameter)
{
  auto* new_parameter =
      new MockExpectedFunctionParameter(static_cast<NamedValue&&>(parameter));
  input_parameters_->add(new_parameter);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_parameter_of_type(
    StringView type,
    StringView name,
    const void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  input_parameters_->add(new_parameter);
  new_parameter->set_const_object_pointer(type, value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_output_parameter_returning(
    StringView name,
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
    StringView type,
    StringView name,
    const void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  output_parameters_->add(new_parameter);
  new_parameter->set_const_object_pointer(type, value);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_unmodified_output_parameter(
    StringView name
)
{
  return with_output_parameter_returning(name, nullptr, 0);
}

ExpectedCall& CheckedExpectedCall::with_captured_parameter(
    StringView name,
    void* value,
    size_t size
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  output_parameters_->add(new_parameter);
  new_parameter->set_value(value);
  new_parameter->set_size(size);
  return *this;
}

ExpectedCall& CheckedExpectedCall::with_captured_parameter_of_type(
    StringView type,
    StringView name,
    void* value
)
{
  NamedValue* new_parameter = new MockExpectedFunctionParameter(name);
  output_parameters_->add(new_parameter);
  new_parameter->set_object_pointer(type, value);
  return *this;
}

bool CheckedExpectedCall::has_input_parameter_with_name(StringView name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return p != nullptr;
}

bool CheckedExpectedCall::has_output_parameter_with_name(StringView name)
{
  NamedValue* p = output_parameters_->get_value_by_name(name);
  return p != nullptr;
}

NamedValue CheckedExpectedCall::get_output_parameter(StringView name)
{
  NamedValue* p = output_parameters_->get_value_by_name(name);
  return (p != nullptr) ? *p : NamedValue("");
}

bool CheckedExpectedCall::are_parameters_matching_actual_call()
{
  NamedValueListNode* p;
  for (p = input_parameters_->begin(); p != nullptr; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      return false;
    }
  }
  for (p = output_parameters_->begin(); p != nullptr; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      return false;
    }
  }
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

  for (p = input_parameters_->begin(); p != nullptr; p = p->next()) {
    item(p)->set_matches_actual_call(false);
  }
  for (p = output_parameters_->begin(); p != nullptr; p = p->next()) {
    item(p)->set_matches_actual_call(false);
  }
}

void CheckedExpectedCall::input_parameter_was_passed(StringView name)
{
  for (NamedValueListNode* p = input_parameters_->begin(); p != nullptr;
       p = p->next()) {
    if (p->get_name() == name) {
      item(p)->set_matches_actual_call(true);
    }
  }
}

void CheckedExpectedCall::output_parameter_was_passed(StringView name)
{
  for (NamedValueListNode* p = output_parameters_->begin(); p != nullptr;
       p = p->next()) {
    if (p->get_name() == name) {
      item(p)->set_matches_actual_call(true);
    }
  }
}

String CheckedExpectedCall::get_input_parameter_value_string(StringView name)
{
  NamedValue* p = input_parameters_->get_value_by_name(name);
  return (p != nullptr) ? string_from(*p) : string_from("failed");
}

bool CheckedExpectedCall::has_input_parameter(const NamedValue& parameter)
{
  NamedValue* p = input_parameters_->get_value_by_name(parameter.get_name());
  return (p != nullptr) ? p->equals(parameter) : ignore_other_parameters_;
}

bool CheckedExpectedCall::has_output_parameter(const NamedValue& parameter)
{
  NamedValue* p = output_parameters_->get_value_by_name(parameter.get_name());
  return (p != nullptr) ? p->compatible_for_copying(parameter)
                        : ignore_other_parameters_;
}

String CheckedExpectedCall::order_to_string() const
{
  if (initial_expected_call_order_ == no_expected_call_order) {
    return {};
  }
  if (initial_expected_call_order_ == final_expected_call_order_) {
    return string_from_format(
        "expected call order: <%u> -> ", initial_expected_call_order_
    );
  }
  return string_from_format(
      "expected calls order: <%u..%u> -> ",
      initial_expected_call_order_,
      final_expected_call_order_
  );
}

String CheckedExpectedCall::call_to_string()
{
  String str;
  if (is_specific_object_expected_) {
    str = string_from_format("(object address: %p)::", object_ptr_);
  }

  str += get_name();
  str += " -> ";
  str += order_to_string();

  if (input_parameters_->begin() == nullptr &&
      output_parameters_->begin() == nullptr) {
    str +=
        (ignore_other_parameters_) ? "all parameters ignored" : "no parameters";
  } else {
    NamedValueListNode* p;

    for (p = input_parameters_->begin(); nullptr != p; p = p->next()) {
      str += string_from_format(
          "%s %s: <%s>",
          p->get_type().c_str(),
          p->get_name().c_str(),
          get_input_parameter_value_string(p->get_name()).c_str()
      );
      if (p->next() != nullptr) {
        str += ", ";
      }
    }

    if ((input_parameters_->begin() != nullptr) &&
        (output_parameters_->begin() != nullptr)) {
      str += ", ";
    }

    for (p = output_parameters_->begin(); p != nullptr; p = p->next()) {
      str += string_from_format(
          "%s %s: <output>", p->get_type().c_str(), p->get_name().c_str()
      );
      if (p->next() != nullptr) {
        str += ", ";
      }
    }

    if (ignore_other_parameters_) {
      str += ", other parameters are ignored";
    }
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

  for (p = input_parameters_->begin(); p != nullptr; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      if (!str.empty()) {
        str += ", ";
      }
      str += string_from_format(
          "%s %s", p->get_type().c_str(), p->get_name().c_str()
      );
    }
  }
  for (p = output_parameters_->begin(); p != nullptr; p = p->next()) {
    if (!item(p)->is_matching_actual_call()) {
      if (!str.empty()) {
        str += ", ";
      }
      str += string_from_format(
          "%s %s", p->get_type().c_str(), p->get_name().c_str()
      );
    }
  }
  return str;
}

bool CheckedExpectedCall::relates_to(StringView function_name)
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
    MockExpectedFunctionParameter(StringView name)
  : NamedValue(name)

{
}

CheckedExpectedCall::MockExpectedFunctionParameter::
    MockExpectedFunctionParameter(NamedValue&& nv)
  : NamedValue(static_cast<NamedValue&&>(nv))

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

ExpectedCall& CheckedExpectedCall::and_return_typed_value(NamedValue value)
{
  return_value_ = static_cast<NamedValue&&>(value);
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
