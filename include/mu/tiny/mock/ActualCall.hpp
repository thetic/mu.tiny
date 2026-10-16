/**
 * @file
 * @brief Fluent interface returned by @ref
 * mu::tiny::mock::Support::actual_call().
 *
 * Inside a mock function implementation, call @ref
 * mu::tiny::mock::Support::actual_call() and then chain @ref
 * mu::tiny::mock::ActualCall::with_parameter() for each argument before
 * querying the return value. The framework matches the actual call against the
 * registered expectations and fails the test if a mismatch is found. See also
 * @ref mu::tiny::mock::ExpectedCall.
 */

#ifndef INCLUDED_MU_TINY_MOCK_ACTUALCALL_HPP
#define INCLUDED_MU_TINY_MOCK_ACTUALCALL_HPP

#include "mu/tiny/mock/NamedValue.hpp"

#include "mu/tiny/StringView.hpp"
#include "mu/tiny/export.h"

namespace mu {
namespace tiny {
namespace mock {

class FailureReporter;

/**
 * @brief Abstract interface for reporting an actual call and its parameters.
 *
 * Each method returns @c *this so that parameter reporting can be chained.
 * Concrete implementations are created internally by Support.
 */
class MUTINY_EXPORT ActualCall
{
public:
  /** Function pointer return type alias for readability. */
  using FunctionPointerReturnValue = void (*)();

  ActualCall() = default;
  virtual ~ActualCall() = default;

  /** @brief Report the function name for this call (used internally).
   * @return *this. */
  virtual ActualCall& with_name(StringView /*name*/) { return *this; }
  /** @brief Set the call order index for strict-ordering checks.
   * @return *this. */
  virtual ActualCall& with_call_order(unsigned int /*call_order*/)
  {
    return *this;
  }

  /**
   * @brief Report a parameter of any supported type.
   *
   * The template creates a NamedValue and passes it to the single
   * with_typed_parameter() virtual method. Overload resolution for
   * NamedValue::set_value() selects the correct storage type.
   *
   * @tparam T     Parameter type (deduced).
   * @param name   Parameter name, must match the expectation.
   * @param value  Parameter value.
   * @return *this for chaining.
   */
  template<typename T>
  ActualCall& with_parameter(StringView name, T value)
  {
    NamedValue nv(name);
    nv.set_value(value);
    return with_typed_parameter(static_cast<NamedValue&&>(nv));
  }

  /**
   * @brief Report a memory buffer parameter.
   *
   * @param name   Parameter name.
   * @param value  Pointer to the buffer.
   * @param size   Buffer size in bytes.
   * @return *this for chaining.
   */
  ActualCall& with_parameter(
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
   * @brief Type-erased virtual entry point for parameter reporting.
   *
   * All non-virtual with_parameter() overloads delegate here after
   * constructing a NamedValue. Subclasses override this single method
   * instead of one method per type.
   *
   * @param parameter  Named, typed parameter value.
   * @return *this for chaining.
   */
  virtual ActualCall& with_typed_parameter(NamedValue parameter) = 0;

  /**
   * @brief Report an object parameter identified by a type name string.
   *
   * The mock framework uses the installed @ref NamedValueComparator for
   * @p type_name to compare against the expectation.
   *
   * @param type_name  Type name, must match the one used in the expectation.
   * @param name       Parameter name.
   * @param value      Const pointer to the object.
   * @return *this for chaining.
   */
  virtual ActualCall& with_parameter_of_type(
      StringView type_name,
      StringView name,
      const void* value
  ) = 0;

  /**
   * @brief Report an output parameter by name.
   *
   * The framework copies the value configured in the expectation into @p output
   * (using the installed copier if the type is non-native).
   *
   * @param name    Parameter name.
   * @param output  Pointer to the caller's output buffer.
   * @return *this for chaining.
   */
  virtual ActualCall& with_output_parameter(StringView name, void* output) = 0;

  /**
   * @brief Report an output parameter with a custom object type.
   *
   * @param type_name  Type name; must match the copier installed via
   * @ref Support::install_copier().
   * @param name       Parameter name.
   * @param output     Pointer to the caller's output buffer.
   * @return *this for chaining.
   */
  virtual ActualCall& with_output_parameter_of_type(
      StringView type_name,
      StringView name,
      void* output
  ) = 0;

  /**
   * @brief Report a captured parameter by name.
   *
   * The framework copies @p value into the buffer configured on the
   * matching expectation via @ref ExpectedCall::with_captured_parameter(),
   * so the test can inspect it after the call. This is the mirror of
   * with_output_parameter(): data flows from the actual call to the test
   * instead of from the expectation to the actual call.
   *
   * @param name   Parameter name.
   * @param value  Pointer to the data to copy into the test's buffer.
   * @return *this for chaining.
   */
  virtual ActualCall& with_captured_parameter(
      StringView name,
      const void* value
  ) = 0;

  /**
   * @brief Report a captured parameter with a custom object type.
   *
   * @param type_name  Type name; must match the copier installed via
   * @ref Support::install_copier().
   * @param name       Parameter name.
   * @param value      Pointer to the object to copy into the test's buffer.
   * @return *this for chaining.
   */
  virtual ActualCall& with_captured_parameter_of_type(
      StringView type_name,
      StringView name,
      const void* value
  ) = 0;

  /** @return true if the matching expectation set a return value. */
  virtual bool has_return_value() = 0;
  /** @return The configured return value as a generic NamedValue. */
  virtual NamedValue return_value() = 0;

  /**
   * @brief Type-safe return-value accessor template.
   *
   * Returns the mock return value converted to @p T.  Because fixed-width
   * types from @c \<stdint.h\> are typedefs for fundamental types, writing
   * @code return_value<int32_t>() @endcode automatically dispatches to the
   * correct getter on every platform.
   *
   * @tparam T  The type to retrieve.
   * @return The stored return value converted to @p T.
   */
  template<typename T>
  T return_value()
  {
    return return_value().get_value<T>();
  }

  /**
   * @brief Type-safe return-value accessor with a fallback default.
   *
   * @tparam T  The type to retrieve.
   * @param default_value  Value returned when no return value was configured.
   * @return The stored return value or @p default_value.
   */
  template<typename T>
  T return_value_or_default(T default_value)
  {
    if (has_return_value()) {
      return return_value().get_value<T>();
    }
    return default_value;
  }

  /**
   * @brief Restrict this actual call to a specific object instance.
   *
   * Matches only the expectation set with the same @p object_ptr via
   * ExpectedCall::on_object().
   *
   * @param object_ptr  Pointer to the object on which the method was called.
   * @return *this for chaining.
   */
  virtual ActualCall& on_object(const void* object_ptr) = 0;
};

} // namespace mock
} // namespace tiny
} // namespace mu

#endif
