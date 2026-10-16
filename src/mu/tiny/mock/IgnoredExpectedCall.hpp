#ifndef INCLUDED_MU_TINY_MOCK_IGNOREDEXPECTEDCALL_HPP
#define INCLUDED_MU_TINY_MOCK_IGNOREDEXPECTEDCALL_HPP

#include "mu/tiny/mock/ExpectedCall.hpp"

#include "mu/tiny/StringView.hpp"

namespace mu {
namespace tiny {
namespace mock {

class IgnoredExpectedCall : public ExpectedCall
{
public:
  ExpectedCall& with_name(StringView /*name*/) override { return *this; }
  ExpectedCall& with_call_order(unsigned int /*order*/) override
  {
    return *this;
  }
  ExpectedCall& with_call_order(
      unsigned int /*min_order*/,
      unsigned int /*max_order*/
  ) override
  {
    return *this;
  }
  ExpectedCall& with_typed_parameter(NamedValue /*parameter*/) override
  {
    return *this;
  }
  ExpectedCall& with_parameter_of_type(
      StringView /*type_name*/,
      StringView /*name*/,
      const void* /*value*/
  ) override
  {
    return *this;
  }
  ExpectedCall& with_output_parameter_returning(
      StringView /*name*/,
      const void* /*value*/,
      size_t /*size*/
  ) override
  {
    return *this;
  }
  ExpectedCall& with_output_parameter_of_type_returning(
      StringView /*type_name*/,
      StringView /*name*/,
      const void* /*value*/
  ) override
  {
    return *this;
  }
  ExpectedCall& with_unmodified_output_parameter(StringView /*name*/) override
  {
    return *this;
  }
  ExpectedCall& with_captured_parameter(
      StringView /*name*/,
      void* /*value*/,
      size_t /*size*/
  ) override
  {
    return *this;
  }
  ExpectedCall& with_captured_parameter_of_type(
      StringView /*type_name*/,
      StringView /*name*/,
      void* /*value*/
  ) override
  {
    return *this;
  }
  ExpectedCall& ignore_other_parameters() override { return *this; }

  ExpectedCall& and_return_typed_value(NamedValue /*value*/) override
  {
    return *this;
  }

  ExpectedCall& on_object(void* /*object_ptr*/) override { return *this; }

  static ExpectedCall& instance();
};

} // namespace mock
} // namespace tiny
} // namespace mu

#endif
