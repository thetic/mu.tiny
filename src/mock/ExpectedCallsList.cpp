#include "mutiny/mock/ExpectedCallsList.hpp"

#include "mutiny/mock/CheckedExpectedCall.hpp"

namespace mu {
namespace tiny {
namespace mock {
using namespace mu::tiny::test;

ExpectedCallsList::~ExpectedCallsList()
{
  while (head_) {
    MockExpectedCallsListNode* next = head_->next;
    delete head_;
    head_ = next;
  }
}

bool ExpectedCallsList::has_calls_out_of_order() const
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (p->expected_call->is_out_of_order())
      return true;
  return false;
}

unsigned int ExpectedCallsList::size() const
{
  unsigned int count = 0;
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    count++;
  return count;
}

bool ExpectedCallsList::empty() const
{
  return head_ == nullptr;
}

unsigned int ExpectedCallsList::amount_of_actual_calls_fulfilled_for(
    const String& name
) const
{
  unsigned int count = 0;
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (p->expected_call->relates_to(name)) {
      count += p->expected_call->get_actual_calls_fulfilled();
    }
  }
  return count;
}

unsigned int ExpectedCallsList::amount_of_unfulfilled_expectations() const
{
  unsigned int count = 0;
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->is_fulfilled())
      count++;
  return count;
}

bool ExpectedCallsList::has_finalized_matching_expectations() const
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (p->expected_call->is_matching_actual_call_and_finalized()) {
      return true;
    }
  }
  return false;
}

bool ExpectedCallsList::has_unfulfilled_expectations() const
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (!p->expected_call->is_fulfilled()) {
      return true;
    }
  }
  return false;
}

bool ExpectedCallsList::has_expectation_with_name(const String& name) const
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (p->expected_call->relates_to(name))
      return true;
  return false;
}

void ExpectedCallsList::add_expected_call(CheckedExpectedCall* call)
{
  auto* new_call = new MockExpectedCallsListNode(call);

  if (head_ == nullptr)
    head_ = new_call;
  else {
    MockExpectedCallsListNode* last_call = head_;
    while (last_call->next)
      last_call = last_call->next;
    last_call->next = new_call;
  }
}

void ExpectedCallsList::add_potentially_matching_expectations(
    const ExpectedCallsList& list
)
{
  for (MockExpectedCallsListNode* p = list.head_; p; p = p->next)
    if (p->expected_call->can_match_actual_calls())
      add_expected_call(p->expected_call);
}

void ExpectedCallsList::add_expectations_related_to(
    const String& name,
    const ExpectedCallsList& list
)
{
  for (MockExpectedCallsListNode* p = list.head_; p; p = p->next)
    if (p->expected_call->relates_to(name))
      add_expected_call(p->expected_call);
}

void ExpectedCallsList::add_expectations(const ExpectedCallsList& list)
{
  for (MockExpectedCallsListNode* p = list.head_; p; p = p->next)
    add_expected_call(p->expected_call);
}

void ExpectedCallsList::only_keep_expectations_related_to(const String& name)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->relates_to(name))
      p->expected_call = nullptr;

  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_out_of_order_expectations()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->is_out_of_order())
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_unmatching_expectations()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (p->expected_call->is_matching_actual_call_and_finalized()) {
      p->expected_call->reset_actual_call_matching_state();
      p->expected_call = nullptr;
    }

  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_expectations_with_input_parameter_name(
    const String& name
)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->has_input_parameter_with_name(name))
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_expectations_with_output_parameter_name(
    const String& name
)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->has_output_parameter_with_name(name))
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_expectations_with_input_parameter(
    const NamedValue& parameter
)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->has_input_parameter(parameter))
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_expectations_with_output_parameter(
    const NamedValue& parameter
)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->has_output_parameter(parameter))
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

void ExpectedCallsList::only_keep_expectations_on_object(const void* object_ptr)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->relates_to_object(object_ptr))
      p->expected_call = nullptr;
  prune_empty_node_from_list();
}

CheckedExpectedCall* ExpectedCallsList::
    remove_first_finalized_matching_expectation()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (p->expected_call->is_matching_actual_call_and_finalized()) {
      CheckedExpectedCall* matching_call = p->expected_call;
      p->expected_call = nullptr;
      prune_empty_node_from_list();
      return matching_call;
    }
  }
  return nullptr;
}

CheckedExpectedCall* ExpectedCallsList::get_first_matching_expectation()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (p->expected_call->is_matching_actual_call()) {
      return p->expected_call;
    }
  }
  return nullptr;
}

CheckedExpectedCall* ExpectedCallsList::remove_first_matching_expectation()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    if (p->expected_call->is_matching_actual_call()) {
      CheckedExpectedCall* matching_call = p->expected_call;
      p->expected_call = nullptr;
      prune_empty_node_from_list();
      return matching_call;
    }
  }
  return nullptr;
}

void ExpectedCallsList::prune_empty_node_from_list()
{
  MockExpectedCallsListNode* current = head_;
  MockExpectedCallsListNode* previous = nullptr;
  MockExpectedCallsListNode* to_be_deleted = nullptr;

  while (current) {
    if (current->expected_call == nullptr) {
      to_be_deleted = current;
      if (previous == nullptr)
        head_ = current = current->next;
      else
        current = previous->next = current->next;
      delete to_be_deleted;
    } else {
      previous = current;
      current = current->next;
    }
  }
}

void ExpectedCallsList::delete_all_expectations_and_clear_list()
{
  while (head_) {
    MockExpectedCallsListNode* next = head_->next;
    delete head_->expected_call;
    delete head_;
    head_ = next;
  }
}

void ExpectedCallsList::reset_actual_call_matching_state()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    p->expected_call->reset_actual_call_matching_state();
}

void ExpectedCallsList::was_passed_to_object()
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    p->expected_call->was_passed_to_object();
}

void ExpectedCallsList::parameter_was_passed(const String& parameter_name)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    p->expected_call->input_parameter_was_passed(parameter_name);
}

void ExpectedCallsList::output_parameter_was_passed(
    const String& parameter_name
)
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    p->expected_call->output_parameter_was_passed(parameter_name);
}

namespace {
String string_or_none_text_when_empty(
    const String& input_string,
    const String& line_prefix
)
{
  String str = input_string;
  if (str == "") {
    str += line_prefix;
    str += MUTINY_STR("<none>");
  }
  return str;
}

String append_string_on_a_new_line(
    const String& input_string,
    const String& line_prefix,
    const String& string_to_append
)
{
  String str = input_string;
  if (str != "")
    str += MUTINY_STR("\n");
  str += line_prefix;
  str += string_to_append;
  return str;
}
} // namespace

String ExpectedCallsList::unfulfilled_calls_to_string(
    const String& line_prefix
) const
{
  String str;
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->is_fulfilled())
      str = append_string_on_a_new_line(
          str, line_prefix, p->expected_call->call_to_string()
      );
  return string_or_none_text_when_empty(str, line_prefix);
}

String ExpectedCallsList::fulfilled_calls_to_string(
    const String& line_prefix
) const
{
  String str;

  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (p->expected_call->is_fulfilled())
      str = append_string_on_a_new_line(
          str, line_prefix, p->expected_call->call_to_string()
      );

  return string_or_none_text_when_empty(str, line_prefix);
}

String ExpectedCallsList::calls_with_missing_parameters_to_string(
    const String& line_prefix,
    const String& missing_parameters_prefix
) const
{
  String str;
  for (MockExpectedCallsListNode* p = head_; p; p = p->next) {
    str = append_string_on_a_new_line(
        str, line_prefix, p->expected_call->call_to_string()
    );
    str = append_string_on_a_new_line(
        str,
        line_prefix + missing_parameters_prefix,
        p->expected_call->missing_parameters_to_string()
    );
  }

  return string_or_none_text_when_empty(str, line_prefix);
}

bool ExpectedCallsList::
    has_unmatching_expectations_because_of_missing_parameters() const
{
  for (MockExpectedCallsListNode* p = head_; p; p = p->next)
    if (!p->expected_call->are_parameters_matching_actual_call())
      return true;
  return false;
}

} // namespace mock
} // namespace tiny
} // namespace mu
