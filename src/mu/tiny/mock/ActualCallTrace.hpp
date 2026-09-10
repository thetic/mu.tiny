#ifndef INCLUDED_MU_TINY_MOCK_ACTUALCALLTRACE_HPP
#define INCLUDED_MU_TINY_MOCK_ACTUALCALLTRACE_HPP

#include "mu/tiny/mock/ActualCall.hpp"

#include "mu/tiny/StringView.hpp"

namespace mu {
namespace tiny {
namespace mock {

class ActualCallTrace : public ActualCall
{
public:
  ActualCallTrace() = default;
  ~ActualCallTrace() override = default;

  ActualCall& with_name(StringView name) override;
  ActualCall& with_call_order(unsigned int /*call_order*/) override;
  ActualCall& with_typed_parameter(NamedValue parameter) override;
  ActualCall& with_parameter_of_type(
      StringView type_name,
      StringView name,
      const void* value
  ) override;
  ActualCall& with_output_parameter(StringView name, void* output) override;
  ActualCall& with_output_parameter_of_type(
      StringView type_name,
      StringView name,
      void* output
  ) override;
  ActualCall& with_captured_parameter(
      StringView name,
      const void* value
  ) override;
  ActualCall& with_captured_parameter_of_type(
      StringView type_name,
      StringView name,
      const void* value
  ) override;

  bool has_return_value() override;
  NamedValue return_value() override;

  ActualCall& on_object(const void* object_ptr) override;

  const char* get_trace_output();
  static ActualCallTrace& instance();
  static void clear_instance();

private:
  String trace_buffer_;

  static ActualCallTrace* instance_;

  void add_parameter_name(StringView name);
};

} // namespace mock
} // namespace tiny
} // namespace mu

#endif
