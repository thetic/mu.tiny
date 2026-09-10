#ifndef INCLUDED_MU_TINY_MOCK_IGNOREDACTUALCALL_HPP
#define INCLUDED_MU_TINY_MOCK_IGNOREDACTUALCALL_HPP

#include "mu/tiny/mock/ActualCall.hpp"

#include "mu/tiny/StringView.hpp"

namespace mu {
namespace tiny {
namespace mock {

class IgnoredActualCall : public ActualCall
{
public:
  ActualCall& with_name(StringView /*name*/) override { return *this; }
  ActualCall& with_call_order(unsigned int /*call_order*/) override
  {
    return *this;
  }

  ActualCall& with_typed_parameter(NamedValue /*parameter*/) override
  {
    return *this;
  }

  ActualCall& with_parameter_of_type(
      StringView /*type_name*/,
      StringView /*name*/,
      const void* /*value*/
  ) override
  {
    return *this;
  }
  ActualCall& with_output_parameter(
      StringView /*name*/,
      void* /*output*/
  ) override
  {
    return *this;
  }
  ActualCall& with_output_parameter_of_type(
      StringView /*type_name*/,
      StringView /*name*/,
      void* /*output*/
  ) override
  {
    return *this;
  }
  ActualCall& with_captured_parameter(
      StringView /*name*/,
      const void* /*value*/
  ) override
  {
    return *this;
  }
  ActualCall& with_captured_parameter_of_type(
      StringView /*type_name*/,
      StringView /*name*/,
      const void* /*value*/
  ) override
  {
    return *this;
  }

  bool has_return_value() override { return false; }
  NamedValue return_value() override { return NamedValue(""); }

  ActualCall& on_object(const void* /*object_ptr*/) override { return *this; }

  static IgnoredActualCall& instance()
  {
    static IgnoredActualCall call;
    return call;
  }
};

} // namespace mock
} // namespace tiny
} // namespace mu

#endif
