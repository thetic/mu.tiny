/**
 * @file
 * @brief Fluent interface returned by @ref
 * mu::tiny::mock::Support::expect_one_call() and friends.
 *
 * Use the fluent methods to constrain which parameter values and call order the
 * actual call must match, and to configure the return value the mock will
 * produce. The corresponding fluent interface for actual calls is
 * @ref mu::tiny::mock::ActualCall.
 */

#ifndef INCLUDED_MU_TINY_MOCK_EXPECTEDCALL_HPP
#define INCLUDED_MU_TINY_MOCK_EXPECTEDCALL_HPP

#include "mu/tiny/mock/NamedValue.hpp"

#include "mu/tiny/StringView.hpp"
#include "mu/tiny/export.h"

namespace mu {
namespace tiny {
namespace mock {

/**
 * @brief Abstract fluent interface for configuring a single call expectation.
 *
 * Each method returns @c *this for chaining. Concrete implementations are
 * created internally by @ref Support. See also @ref ActualCall.
 */
class MUTINY_EXPORT ExpectedCall
{
public:
  ExpectedCall() = default;
  virtual ~ExpectedCall() = default;

  /** @brief Set the expected function name. @param name Function name. @return
   * *this. */
  virtual ExpectedCall& with_name(StringView name) = 0;

  /**
   * @brief Require this call to happen at a specific position in call order.
   *
   * @param order  Expected call order index.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_call_order(unsigned int order) = 0;

  /**
   * @brief Require this call to happen within a range of call order positions.
   *
   * @param min_order  Earliest acceptable order index.
   * @param max_order  Latest acceptable order index.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_call_order(
      unsigned int min_order,
      unsigned int max_order
  ) = 0;

  /**
   * @brief Constrain a parameter of any supported type.
   *
   * The template creates a NamedValue and passes it to the single
   * with_typed_parameter() virtual method. Overload resolution for
   * NamedValue::set_value() selects the correct storage type.
   *
   * @tparam T     Parameter type (deduced).
   * @param name   Parameter name, must match what the mock implementation
   * reports.
   * @param value  Expected parameter value.
   * @return *this for chaining.
   */
  template<typename T>
  ExpectedCall& with_parameter(StringView name, T value)
  {
    NamedValue nv(name);
    nv.set_value(value);
    return with_typed_parameter(static_cast<NamedValue&&>(nv));
  }

  /**
   * @brief Constrain a double parameter with an explicit tolerance.
   *
   * @param name       Parameter name.
   * @param value      Expected double value.
   * @param tolerance  Maximum allowed absolute difference from @p value.
   * @return *this for chaining.
   */
  ExpectedCall& with_parameter(StringView name, double value, double tolerance)
  {
    NamedValue nv(name);
    nv.set_value(value, tolerance);
    return with_typed_parameter(static_cast<NamedValue&&>(nv));
  }

  /**
   * @brief Constrain a memory buffer parameter.
   *
   * @param name   Parameter name.
   * @param value  Pointer to the expected buffer content.
   * @param size   Number of bytes to compare.
   * @return *this for chaining.
   */
  ExpectedCall& with_parameter(
      StringView name,
      const unsigned char* value,
      size_t size
  )
  {
    NamedValue nv(name);
    nv.set_memory_buffer(value, size);
    return with_typed_parameter(static_cast<NamedValue&&>(nv));
  }

  /**
   * @brief Type-erased virtual entry point for parameter constraints.
   *
   * All non-virtual with_parameter() overloads delegate here after
   * constructing a NamedValue. Subclasses override this single method
   * instead of one method per type.
   *
   * @param parameter  Named, typed parameter value.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_typed_parameter(NamedValue parameter) = 0;

  /**
   * @brief Constrain an object parameter identified by a type name string.
   *
   * Uses the NamedValueComparator installed for @p type_name to compare
   * the expected value against the actual call's parameter.
   *
   * @param type_name  Type name; must match the comparator key and the actual
   * call.
   * @param name       Parameter name.
   * @param value      Const pointer to the expected object.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_parameter_of_type(
      StringView type_name,
      StringView name,
      const void* value
  ) = 0;

  /**
   * @brief Configure an output parameter: the framework will copy @p value into
   *        the caller's buffer when the actual call reports this parameter.
   *
   * @param name   Parameter name.
   * @param value  Pointer to the data to copy out (plain memcpy).
   * @param size   Number of bytes to copy.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_output_parameter_returning(
      StringView name,
      const void* value,
      size_t size
  ) = 0;

  /**
   * @brief Configure an output parameter for a custom object type.
   *
   * Uses the NamedValueCopier installed for @p type_name to copy the
   * object into the caller's buffer.
   *
   * @param type_name  Type name; must match the copier key.
   * @param name       Parameter name.
   * @param value      Pointer to the expected object to copy out.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_output_parameter_of_type_returning(
      StringView type_name,
      StringView name,
      const void* value
  ) = 0;

  /**
   * @brief Expect an output parameter to be passed but leave the buffer
   * unmodified.
   *
   * @param name  Parameter name.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_unmodified_output_parameter(StringView name) = 0;

  /**
   * @brief Capture a parameter: the framework will copy the value the actual
   *        call reports for this parameter into @p value (plain memcpy).
   *
   * Use this instead of with_parameter_of_type() when the parameter's value
   * can't be predicted ahead of time (a generated id, a timestamp, a struct
   * populated by the code under test) and the test needs to inspect it after
   * the call rather than compare it during the call.
   *
   * @param name   Parameter name.
   * @param value  Pointer to the buffer to copy the actual value into.
   * @param size   Number of bytes to copy.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_captured_parameter(
      StringView name,
      void* value,
      size_t size
  ) = 0;

  /**
   * @brief Capture a parameter of a custom object type.
   *
   * Uses the NamedValueCopier installed for @p type_name to copy the actual
   * object into @p value.
   *
   * @param type_name  Type name; must match the copier key.
   * @param name       Parameter name.
   * @param value      Pointer to the buffer to copy the actual object into.
   * @return *this for chaining.
   */
  virtual ExpectedCall& with_captured_parameter_of_type(
      StringView type_name,
      StringView name,
      void* value
  ) = 0;

  /**
   * @brief Allow the actual call to pass parameters not listed in this
   * expectation.
   *
   * By default every parameter must be explicitly listed.
   *
   * @return *this for chaining.
   */
  virtual ExpectedCall& ignore_other_parameters() = 0;

  /**
   * @brief Configure the return value the mock will produce.
   *
   * The template creates a NamedValue and passes it to the single
   * and_return_typed_value() virtual method.
   *
   * @tparam T     Return value type (deduced).
   * @param value  Value to return when this expectation is matched.
   * @return *this for chaining.
   */
  template<typename T>
  ExpectedCall& and_return_value(T value)
  {
    NamedValue nv("returnValue");
    nv.set_value(value);
    return and_return_typed_value(static_cast<NamedValue&&>(nv));
  }

  /**
   * @brief Type-erased virtual entry point for return-value configuration.
   *
   * All non-virtual and_return_value() overloads delegate here after
   * constructing a NamedValue. Subclasses override this single method
   * instead of one method per type.
   *
   * @param value  Named return value.
   * @return *this for chaining.
   */
  virtual ExpectedCall& and_return_typed_value(NamedValue value) = 0;

  /**
   * @brief Restrict this expectation to calls made on a specific object.
   *
   * Only an actual call that reports the same @p object_ptr via
   * ActualCall::on_object() will match this expectation.
   *
   * @param object_ptr  Pointer to the expected object instance.
   * @return *this for chaining.
   */
  virtual ExpectedCall& on_object(void* object_ptr) = 0;
};

} // namespace mock

/**
 * @brief Produce a display string for a NamedValue.
 *
 * Used internally by the failure reporter to format expected vs actual output.
 *
 * @param parameter  Value to convert.
 * @return Human-readable string representation.
 */
String string_from(const mock::NamedValue& parameter);

} // namespace tiny
} // namespace mu

#endif
