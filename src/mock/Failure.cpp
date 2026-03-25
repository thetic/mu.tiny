#include "mutiny/mock/Failure.hpp"

#include "mutiny/mock/ExpectedCall.hpp"
#include "mutiny/mock/ExpectedCallsList.hpp"
#include "mutiny/mock/NamedValue.hpp"

#include "mutiny/test/Shell.hpp"

namespace mu {
namespace tiny {
namespace mock {

class FailureReporterTerminator : public test::Terminator
{
public:
  FailureReporterTerminator(bool crash_on_failure)
    : crash_on_failure_(crash_on_failure)
  {
  }

  void exit_current_test() const override
  {
    if (crash_on_failure_)
      test::Shell::crash();

    test::Shell::get_current_test_terminator().exit_current_test();
  }

  ~FailureReporterTerminator() override = default;

private:
  bool crash_on_failure_;
};

void FailureReporter::report_failure(const Failure& failure)
{
  if (!get_test_to_fail()->has_failed())
    get_test_to_fail()->add_failure(failure);
}

void FailureReporter::exit_test()
{
  FailureReporterTerminator(crash_on_failure_).exit_current_test();
}

void FailureReporter::fail_test(Failure failure)
{
  if (!get_test_to_fail()->has_failed()) {
    report_failure(failure);
    {
      Failure f(static_cast<Failure&&>(failure));
    }
    exit_test();
  }
}

test::Shell* FailureReporter::get_test_to_fail()
{
  return test::Shell::get_current();
}

Failure::Failure(test::Shell* test)
  : test::Failure(
        test,
        "Test failed with Failure without an error! Something went "
        "seriously wrong."
    )
{
}

void Failure::add_expectations_and_call_history(
    const ExpectedCallsList& expectations
)
{
  message_ += MUTINY_STR("\tEXPECTED calls that WERE NOT fulfilled:\n");
  message_ += expectations.unfulfilled_calls_to_string("\t\t");
  message_ += MUTINY_STR("\n\tEXPECTED calls that WERE fulfilled:\n");
  message_ += expectations.fulfilled_calls_to_string("\t\t");
}

void Failure::add_expectations_and_call_history_related_to(
    const test::String& name,
    const ExpectedCallsList& expectations
)
{
  ExpectedCallsList expectations_for_function;
  expectations_for_function.add_expectations_related_to(name, expectations);

  message_ += MUTINY_STR(
      "\tEXPECTED calls that WERE NOT fulfilled related to function: "
  );
  message_ += name;
  message_ += MUTINY_STR("\n");

  message_ += expectations_for_function.unfulfilled_calls_to_string("\t\t");

  message_ += MUTINY_STR(
      "\n\tEXPECTED calls that WERE fulfilled related to function: "
  );
  message_ += name;
  message_ += MUTINY_STR("\n");

  message_ += expectations_for_function.fulfilled_calls_to_string("\t\t");
}

ExpectedCallsDidntHappenFailure::ExpectedCallsDidntHappenFailure(
    test::Shell* test,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  message_ = "Mock Failure: Expected call WAS NOT fulfilled.\n";
  add_expectations_and_call_history(expectations);
}

UnexpectedCallHappenedFailure::UnexpectedCallHappenedFailure(
    test::Shell* test,
    const test::String& name,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  unsigned int amount_of_actual_calls =
      expectations.amount_of_actual_calls_fulfilled_for(name);
  if (amount_of_actual_calls > 0) {
    test::String ordinal_number =
        test::string_from_ordinal_number(amount_of_actual_calls + 1);
    message_ = test::string_from_format(
        "Mock Failure: Unexpected additional (%s) call to function: ",
        ordinal_number.c_str()
    );
  } else {
    message_ = "Mock Failure: Unexpected call to function: ";
  }
  message_ += name;
  message_ += MUTINY_STR("\n");
  add_expectations_and_call_history(expectations);
}

CallOrderFailure::CallOrderFailure(
    test::Shell* test,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  ExpectedCallsList expectations_for_out_of_order;
  expectations_for_out_of_order.add_expectations(expectations);
  expectations_for_out_of_order.only_keep_out_of_order_expectations();

  message_ = "Mock Failure: Out of order calls";
  message_ += MUTINY_STR("\n");
  add_expectations_and_call_history(expectations_for_out_of_order);
}

UnexpectedInputParameterFailure::UnexpectedInputParameterFailure(
    test::Shell* test,
    const test::String& function_name,
    NamedValue parameter,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  ExpectedCallsList expectations_for_function_with_parameter_name;
  expectations_for_function_with_parameter_name.add_expectations_related_to(
      function_name, expectations
  );
  expectations_for_function_with_parameter_name
      .only_keep_expectations_with_input_parameter_name(parameter.get_name());

  if (expectations_for_function_with_parameter_name.empty()) {
    message_ = "Mock Failure: Unexpected parameter name to function \"";
    message_ += function_name;
    message_ += MUTINY_STR("\": ");
    message_ += parameter.get_name();
  } else {
    message_ = "Mock Failure: Unexpected parameter value to parameter \"";
    message_ += parameter.get_name();
    message_ += MUTINY_STR("\" to function \"");
    message_ += function_name;
    message_ += MUTINY_STR("\": <");
    message_ += string_from(parameter);
    message_ += MUTINY_STR(">");
  }

  message_ += MUTINY_STR("\n");
  add_expectations_and_call_history_related_to(function_name, expectations);

  message_ +=
      MUTINY_STR("\n\tACTUAL unexpected parameter passed to function: ");
  message_ += function_name;
  message_ += MUTINY_STR("\n");

  message_ += MUTINY_STR("\t\t");
  message_ += parameter.get_type();
  message_ += MUTINY_STR(" ");
  message_ += parameter.get_name();
  message_ += MUTINY_STR(": <");
  message_ += string_from(parameter);
  message_ += MUTINY_STR(">");
}

UnexpectedOutputParameterFailure::UnexpectedOutputParameterFailure(
    test::Shell* test,
    const test::String& function_name,
    NamedValue parameter,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  ExpectedCallsList expectations_for_function_with_parameter_name;
  expectations_for_function_with_parameter_name.add_expectations_related_to(
      function_name, expectations
  );
  expectations_for_function_with_parameter_name
      .only_keep_expectations_with_output_parameter_name(parameter.get_name());

  if (expectations_for_function_with_parameter_name.empty()) {
    message_ = "Mock Failure: Unexpected output parameter name to function \"";
    message_ += function_name;
    message_ += MUTINY_STR("\": ");
    message_ += parameter.get_name();
  } else {
    message_ = "Mock Failure: Unexpected parameter type \"";
    message_ += parameter.get_type();
    message_ += MUTINY_STR("\" to output parameter \"");
    message_ += parameter.get_name();
    message_ += MUTINY_STR("\" to function \"");
    message_ += function_name;
    message_ += MUTINY_STR("\"");
  }

  message_ += MUTINY_STR("\n");
  add_expectations_and_call_history_related_to(function_name, expectations);

  message_ +=
      MUTINY_STR("\n\tACTUAL unexpected output parameter passed to function: ");
  message_ += function_name;
  message_ += MUTINY_STR("\n");

  message_ += MUTINY_STR("\t\t");
  message_ += parameter.get_type();
  message_ += MUTINY_STR(" ");
  message_ += parameter.get_name();
}

ExpectedParameterDidntHappenFailure::ExpectedParameterDidntHappenFailure(
    test::Shell* test,
    const test::String& function_name,
    const ExpectedCallsList& all_expectations,
    const ExpectedCallsList& matching_expectations
)
  : Failure(test)
{
  message_ = "Mock Failure: Expected parameter for function \"";
  message_ += function_name;
  message_ += MUTINY_STR("\" did not happen.\n");

  message_ += MUTINY_STR(
      "\tEXPECTED calls with MISSING parameters related to function: "
  );
  message_ += function_name;
  message_ += MUTINY_STR("\n");
  message_ += matching_expectations.calls_with_missing_parameters_to_string(
      "\t\t", "\tMISSING parameters: "
  );
  message_ += MUTINY_STR("\n");

  add_expectations_and_call_history_related_to(function_name, all_expectations);
}

NoWayToCompareCustomTypeFailure::NoWayToCompareCustomTypeFailure(
    test::Shell* test,
    test::String type_name
)
  : Failure(test)
{
  message_ = test::string_from_format(
      "Failure: No way to compare type <%s>. "
      "Please install a NamedValueComparator.",
      type_name.c_str()
  );
}

NoWayToCopyCustomTypeFailure::NoWayToCopyCustomTypeFailure(
    test::Shell* test,
    test::String type_name
)
  : Failure(test)
{
  message_ = test::string_from_format(
      "Failure: No way to copy type <%s>. Please "
      "install a NamedValueCopier.",
      type_name.c_str()
  );
}

UnexpectedObjectFailure::UnexpectedObjectFailure(
    test::Shell* test,
    const test::String& function_name,
    const void* actual,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  message_ = test::string_from_format(
      "Failure: Function called on an unexpected object: %s\n"
      "\tActual object for call has address: <%p>\n",
      function_name.c_str(),
      actual
  );
  add_expectations_and_call_history_related_to(function_name, expectations);
}

ExpectedObjectDidntHappenFailure::ExpectedObjectDidntHappenFailure(
    test::Shell* test,
    const test::String& function_name,
    const ExpectedCallsList& expectations
)
  : Failure(test)
{
  message_ = test::string_from_format(
      "Mock Failure: Expected call on object for "
      "function \"%s\" but it did not happen.\n",
      function_name.c_str()
  );
  add_expectations_and_call_history_related_to(function_name, expectations);
}

} // namespace mock
} // namespace tiny
} // namespace mu
