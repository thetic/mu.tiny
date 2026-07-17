#include "mu/tiny/mock/CheckedActualCall.hpp"

#include "mu/tiny/mock/CheckedExpectedCall.hpp"
#include "mu/tiny/mock/Failure.hpp"
#include "mu/tiny/mock/NamedValueCopier.hpp"

#include "mu/tiny/test/Shell.hpp"

#include <string.h>

namespace mu {
namespace tiny {
namespace mock {

const String& CheckedActualCall::get_name() const
{
  return function_name_;
}

CheckedActualCall::CheckedActualCall(
    unsigned int call_order,
    FailureReporter* reporter,
    const ExpectedCallsList& all_expectations
)
  : call_order_(call_order)
  , reporter_(reporter)
  , all_expectations_(all_expectations)

{
  potentially_matching_expectations_.add_potentially_matching_expectations(
      all_expectations
  );
}

CheckedActualCall::~CheckedActualCall()
{
  clean_up_output_parameter_list();
}

void CheckedActualCall::set_mock_failure_reporter(FailureReporter* reporter)
{
  reporter_ = reporter;
}

test::Shell* CheckedActualCall::get_test() const
{
  return reporter_->get_test_to_fail();
}

void CheckedActualCall::fail_with(Failure failure)
{
  if (!has_failed()) {
    set_state(MutinyActualCallState::failed);
    reporter_->fail_test(static_cast<Failure&&>(failure));
  }
}

void CheckedActualCall::copy_output_parameters(
    CheckedExpectedCall* expected_call
)
{
  for (MockOutputParametersListNode* p = output_parameter_expectations_;
       p != nullptr;
       p = p->next) {
    NamedValue output_parameter = expected_call->get_output_parameter(p->name);
    NamedValueCopier* copier = output_parameter.get_copier();
    if (copier != nullptr) {
      copier->copy(p->ptr, output_parameter.get_const_object_pointer());
    } else if (
        (output_parameter.get_type() == "const void*") && (p->type == "void*")
    ) {
      const void* data = output_parameter.get_value<const void*>();
      size_t size = output_parameter.get_size();
      memcpy(p->ptr, data, size);
    } else if (!output_parameter.get_name().empty()) {
      fail_with(NoWayToCopyCustomTypeFailure(
          get_test(),
          expected_call->get_output_parameter(p->name).get_type().c_str()
      ));
    }
  }
}

void CheckedActualCall::complete_call_when_match_is_found()
{
  // Expectations that don't ignore parameters have higher fulfillment
  // preference than those that ignore parameters

  matching_expectation_ = potentially_matching_expectations_
                              .remove_first_finalized_matching_expectation();
  if (matching_expectation_ != nullptr) {
    copy_output_parameters(matching_expectation_);
    call_has_succeeded();
  } else {
    CheckedExpectedCall* matching_expectation_with_ignored_parameters =
        potentially_matching_expectations_.get_first_matching_expectation();
    if (matching_expectation_with_ignored_parameters != nullptr) {
      copy_output_parameters(matching_expectation_with_ignored_parameters);
    }
  }
}

void CheckedActualCall::call_has_succeeded()
{
  set_state(MutinyActualCallState::success);
}

void CheckedActualCall::discard_currently_matching_expectations()
{
  if (matching_expectation_ != nullptr) {
    matching_expectation_->reset_actual_call_matching_state();
    matching_expectation_ = nullptr;
  }
  potentially_matching_expectations_.only_keep_unmatching_expectations();
}

void CheckedActualCall::set_name_and_check(String name)
{
  function_name_ = static_cast<String&&>(name);
  set_state(MutinyActualCallState::in_progress);

  potentially_matching_expectations_.only_keep_expectations_related_to(
      function_name_
  );
  if (potentially_matching_expectations_.empty()) {
    fail_with(UnexpectedCallHappenedFailure(
        get_test(), function_name_, all_expectations_
    ));
    return;
  }

  complete_call_when_match_is_found();
}

void CheckedActualCall::check_input_parameter(NamedValue actual_parameter)
{
  if (has_failed()) {
    return;
  }

  set_state(MutinyActualCallState::in_progress);
  discard_currently_matching_expectations();

  potentially_matching_expectations_
      .only_keep_expectations_with_input_parameter(actual_parameter);

  if (potentially_matching_expectations_.empty()) {
    UnexpectedInputParameterFailure failure(
        get_test(),
        get_name(),
        static_cast<NamedValue&&>(actual_parameter),
        all_expectations_
    );
    fail_with(static_cast<Failure&&>(failure));
    return;
  }

  potentially_matching_expectations_.parameter_was_passed(
      actual_parameter.get_name()
  );
  complete_call_when_match_is_found();
}

void CheckedActualCall::check_output_parameter(NamedValue output_parameter)
{
  if (has_failed()) {
    return;
  }

  set_state(MutinyActualCallState::in_progress);
  discard_currently_matching_expectations();

  potentially_matching_expectations_
      .only_keep_expectations_with_output_parameter(output_parameter);

  if (potentially_matching_expectations_.empty()) {
    UnexpectedOutputParameterFailure failure(
        get_test(),
        get_name(),
        static_cast<NamedValue&&>(output_parameter),
        all_expectations_
    );
    fail_with(static_cast<Failure&&>(failure));
    return;
  }

  potentially_matching_expectations_.output_parameter_was_passed(
      output_parameter.get_name()
  );
  complete_call_when_match_is_found();
}

ActualCall& CheckedActualCall::with_typed_parameter(NamedValue parameter)
{
  check_input_parameter(static_cast<NamedValue&&>(parameter));
  return *this;
}

ActualCall& CheckedActualCall::with_parameter_of_type(
    StringView type,
    StringView name,
    const void* value
)
{
  NamedValue actual_parameter(name);
  actual_parameter.set_const_object_pointer(type, value);

  if (actual_parameter.get_comparator() == nullptr) {
    {
      NamedValue freed(static_cast<NamedValue&&>(actual_parameter));
    }
    fail_with(NoWayToCompareCustomTypeFailure(
        get_test(), String(type.data(), type.size())
    ));
    return *this;
  }
  check_input_parameter(static_cast<NamedValue&&>(actual_parameter));
  return *this;
}

ActualCall& CheckedActualCall::with_output_parameter(
    StringView name,
    void* output
)
{
  add_output_parameter(name, "void*", output);

  NamedValue output_parameter(name);
  output_parameter.set_value(output);
  check_output_parameter(static_cast<NamedValue&&>(output_parameter));

  return *this;
}

ActualCall& CheckedActualCall::with_output_parameter_of_type(
    StringView type,
    StringView name,
    void* output
)
{
  add_output_parameter(name, type, output);

  NamedValue output_parameter(name);
  output_parameter.set_const_object_pointer(type, output);
  check_output_parameter(static_cast<NamedValue&&>(output_parameter));

  return *this;
}

bool CheckedActualCall::is_fulfilled() const
{
  return state_ == MutinyActualCallState::success;
}

bool CheckedActualCall::has_failed() const
{
  return state_ == MutinyActualCallState::failed;
}

void CheckedActualCall::check_expectations()
{
  if (expectations_checked_) {
    return;
  }

  expectations_checked_ = true;

  if (state_ != MutinyActualCallState::in_progress) {
    if (state_ == MutinyActualCallState::success) {
      matching_expectation_->call_was_made(call_order_);
    }
    potentially_matching_expectations_.reset_actual_call_matching_state();
    return;
  }

  matching_expectation_ =
      potentially_matching_expectations_.remove_first_matching_expectation();
  if (matching_expectation_ != nullptr) {
    matching_expectation_->finalize_actual_call_match();
    call_has_succeeded();
    matching_expectation_->call_was_made(call_order_);
    potentially_matching_expectations_.reset_actual_call_matching_state();
    return;
  }

  if (potentially_matching_expectations_
          .has_unmatching_expectations_because_of_missing_parameters()) {
    fail_with(ExpectedParameterDidntHappenFailure(
        get_test(),
        get_name(),
        all_expectations_,
        potentially_matching_expectations_
    ));
  } else {
    fail_with(ExpectedObjectDidntHappenFailure(
        get_test(), get_name(), all_expectations_
    ));
  }
}

void CheckedActualCall::set_state(MutinyActualCallState state)
{
  state_ = state;
}

NamedValue CheckedActualCall::return_value()
{
  check_expectations();
  if (matching_expectation_ != nullptr) {
    return matching_expectation_->return_value();
  }
  return NamedValue("no return value");
}

bool CheckedActualCall::has_return_value()
{
  return !return_value().get_name().empty();
}

ActualCall& CheckedActualCall::on_object(const void* object_ptr)
{
  if (has_failed()) {
    return *this;
  }

  // Currently matching expectations are not discarded because the passed object
  // is ignored if not specifically set in the expectation

  potentially_matching_expectations_.only_keep_expectations_on_object(
      object_ptr
  );

  if ((matching_expectation_ == nullptr) &&
      potentially_matching_expectations_.empty()) {
    fail_with(UnexpectedObjectFailure(
        get_test(), get_name(), object_ptr, all_expectations_
    ));
    return *this;
  }

  potentially_matching_expectations_.was_passed_to_object();

  if (matching_expectation_ == nullptr) {
    complete_call_when_match_is_found();
  }

  return *this;
}

void CheckedActualCall::add_output_parameter(
    StringView name,
    StringView type,
    void* ptr
)
{
  auto* new_node = new MockOutputParametersListNode(name, type, ptr);

  if (output_parameter_expectations_ == nullptr) {
    output_parameter_expectations_ = new_node;
  } else {
    MockOutputParametersListNode* last_node = output_parameter_expectations_;
    while (last_node->next != nullptr) {
      last_node = last_node->next;
    }
    last_node->next = new_node;
  }
}

void CheckedActualCall::clean_up_output_parameter_list()
{
  MockOutputParametersListNode* current = output_parameter_expectations_;
  MockOutputParametersListNode* to_be_deleted = nullptr;

  while (current != nullptr) {
    to_be_deleted = current;
    output_parameter_expectations_ = current = current->next;
    delete to_be_deleted;
  }
}

} // namespace mock
} // namespace tiny
} // namespace mu
