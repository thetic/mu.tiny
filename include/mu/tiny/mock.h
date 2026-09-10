#ifndef INCLUDED_MU_TINY_MOCK_H
#define INCLUDED_MU_TINY_MOCK_H

/**
 * @file
 * @brief C interface to the mutiny mock framework.
 *
 * Provides a vtable-style C API for mock interaction from pure C test code.
 * Call @ref mutiny_mock() to get the global mock scope, or @ref
 * mutiny_mock_scope() for a named scope; then use the function pointers on the
 * returned struct to set up expectations and report actual calls.
 *
 * @see mutiny_mock()
 * @see mutiny_mock_scope()
 */

#include "mu/tiny/export.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief Discriminator for the type stored in a @ref MutinyMockValue.
   *
   * Each enumerator corresponds to one field of the @ref MutinyMockValue union.
   */
  enum MutinyMockValueType
  {
    mutiny_type_bool,                       /**< Boolean (stored as int). */
    mutiny_type_unsigned_integer,           /**< unsigned int */
    mutiny_type_integer,                    /**< int */
    mutiny_type_long_integer,               /**< long int */
    mutiny_type_unsigned_long_integer,      /**< unsigned long int */
    mutiny_type_long_long_integer,          /**< long long */
    mutiny_type_unsigned_long_long_integer, /**< unsigned long long */
    mutiny_type_double,                     /**< double */
    mutiny_type_string,                     /**< const char* */
    mutiny_type_pointer,                    /**< void* */
    mutiny_type_const_pointer,              /**< const void* */
    mutiny_type_functionpointer,            /**< void (*)(void) */
    mutiny_type_memorybuffer,               /**< const unsigned char* */
    mutiny_type_object,                     /**< void* (custom object) */
    mutiny_type_const_object /**< const void* (const custom object) */
  };

  /**
   * @brief Tagged union that carries a single mock parameter or return value.
   *
   * Read the field matching @c type; all other fields are undefined.
   */
  struct MutinyMockValue
  {
    enum MutinyMockValueType
        type; /**< Discriminator: which union member is valid. */
    union
    {
      int bool_value;
      int int_value;
      unsigned int unsigned_int_value;
      long int long_int_value;
      unsigned long int unsigned_long_int_value;
      long long long_long_int_value;
      unsigned long long unsigned_long_long_int_value;
      double double_value;
      const char* string_value;
      void* pointer_value;
      const void* const_pointer_value;
      void (*function_pointer_value)(void);
      const unsigned char* memory_buffer_value;
      void* object_value;
      const void* const_object_value;
    } value; /**< The stored value; valid field determined by @c type. */
  };

  /**
   * @brief Vtable for reporting an actual mock call from C code.
   *
   * Obtained by calling the actual_call() function pointer on a
   * @ref MutinyMockSupport. Chain the @c with_*_parameters function pointers to
   * supply parameter values, then call a return-value accessor.
   */
  struct MutinyMockActualCall
  {
    /**
     * @brief Report a bool parameter.
     * @param name Parameter name.
     * @param value Value (non-zero = true).
     * @return this.
     */
    struct MutinyMockActualCall* (*with_bool_parameters)(
        const char* name,
        int value
    );

    /**
     * @brief Report an int parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_int_parameters)(
        const char* name,
        int value
    );

    /**
     * @brief Report an unsigned int parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_unsigned_int_parameters)(
        const char* name,
        unsigned int value
    );

    /**
     * @brief Report a long int parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_long_int_parameters)(
        const char* name,
        long int value
    );

    /**
     * @brief Report an unsigned long int parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_unsigned_long_int_parameters)(
        const char* name,
        unsigned long int value
    );

    /**
     * @brief Report a long long parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_long_long_int_parameters)(
        const char* name,
        long long value
    );

    /**
     * @brief Report an unsigned long long parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_unsigned_long_long_int_parameters)(
        const char* name,
        unsigned long long value
    );

    /**
     * @brief Report a double parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_double_parameters)(
        const char* name,
        double value
    );

    /**
     * @brief Report a string parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_string_parameters)(
        const char* name,
        const char* value
    );

    /**
     * @brief Report a @c void* parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_pointer_parameters)(
        const char* name,
        void* value
    );

    /**
     * @brief Report a <tt>const void*</tt> parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_const_pointer_parameters)(
        const char* name,
        const void* value
    );

    /**
     * @brief Report a function pointer parameter.
     * @param name Parameter name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_function_pointer_parameters)(
        const char* name,
        void (*value)(void)
    );

    /**
     * @brief Report a memory buffer parameter.
     * @param name Parameter name.
     * @param value Buffer pointer.
     * @param size Buffer size.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_memory_buffer_parameter)(
        const char* name,
        const unsigned char* value,
        size_t size
    );

    /**
     * @brief Report an object parameter identified by type name.
     * @param type Type name.
     * @param name Parameter name.
     * @param value Object pointer.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_parameter_of_type)(
        const char* type,
        const char* name,
        const void* value
    );

    /**
     * @brief Report an output parameter.
     * @param name Parameter name.
     * @param value Output buffer.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_output_parameter)(
        const char* name,
        void* value
    );

    /**
     * @brief Report a custom-type output parameter.
     * @param type Type name.
     * @param name Parameter name.
     * @param value Output buffer.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_output_parameter_of_type)(
        const char* type,
        const char* name,
        void* value
    );

    /**
     * @brief Report a captured parameter.
     * @param name Parameter name.
     * @param value Pointer to the data to copy into the test's buffer.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_captured_parameter)(
        const char* name,
        const void* value
    );

    /**
     * @brief Report a custom-type captured parameter.
     * @param type Type name.
     * @param name Parameter name.
     * @param value Pointer to the object to copy into the test's buffer.
     * @return this.
     */
    struct MutinyMockActualCall* (*with_captured_parameter_of_type)(
        const char* type,
        const char* name,
        const void* value
    );

    /** @return Non-zero if the expectation set a return value. */
    int (*has_return_value)(void);

    /** @return The configured return value as a tagged union. */
    struct MutinyMockValue (*return_value)(void);

    /** @return The configured bool return value (as int). */
    int (*bool_return_value)(void);

    /**
     * @return The bool return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    int (*return_bool_value_or_default)(int default_value);

    /** @return The configured int return value. */
    int (*int_return_value)(void);

    /**
     * @return The int return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    int (*return_int_value_or_default)(int default_value);

    /** @return The configured unsigned int return value. */
    unsigned int (*unsigned_int_return_value)(void);

    /**
     * @return The unsigned int return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    unsigned int (*return_unsigned_int_value_or_default)(
        unsigned int default_value
    );

    /** @return The configured long int return value. */
    long int (*long_int_return_value)(void);

    /**
     * @return The long int return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    long int (*return_long_int_value_or_default)(long int default_value);

    /** @return The configured unsigned long int return value. */
    unsigned long int (*unsigned_long_int_return_value)(void);

    /**
     * @return The unsigned long int return value if set, otherwise @p
     * default_value.
     * @param default_value Fallback.
     */
    unsigned long int (*return_unsigned_long_int_value_or_default)(
        unsigned long int default_value
    );

    /** @return The configured long long return value. */
    long long (*long_long_int_return_value)(void);

    /**
     * @return The long long return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    long long (*return_long_long_int_value_or_default)(long long default_value);

    /** @return The configured unsigned long long return value. */
    unsigned long long (*unsigned_long_long_int_return_value)(void);

    /**
     * @return The unsigned long long return value if set, otherwise @p
     * default_value.
     * @param default_value Fallback.
     */
    unsigned long long (*return_unsigned_long_long_int_value_or_default)(
        unsigned long long default_value
    );

    /** @return The configured C string return value. */
    const char* (*string_return_value)(void);

    /**
     * @return The string return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    const char* (*return_string_value_or_default)(const char* default_value);

    /** @return The configured double return value. */
    double (*double_return_value)(void);

    /**
     * @return The double return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    double (*return_double_value_or_default)(double default_value);

    /** @return The configured void* return value. */
    void* (*pointer_return_value)(void);

    /**
     * @return The pointer return value if set, otherwise @p default_value.
     * @param default_value Fallback.
     */
    void* (*return_pointer_value_or_default)(void* default_value);

    /** @return The configured const void* return value. */
    const void* (*const_pointer_return_value)(void);

    /**
     * @return The const pointer return value if set, otherwise @p
     * default_value.
     * @param default_value Fallback.
     */
    const void* (*return_const_pointer_value_or_default)(
        const void* default_value
    );

    /** @return The configured function pointer return value. */
    void (*(*function_pointer_return_value)(void))(void);

    /**
     * @return The function pointer return value if set, otherwise @p
     * default_value.
     * @param default_value Fallback.
     */
    void (*(*return_function_pointer_value_or_default)(void (*default_value)(
        void
    )))(void);
  };

  /**
   * @brief Vtable for recording a call expectation from C code.
   *
   * Obtained from @ref MutinyMockSupport::expect_one_call() or similar. Chain
   * the
   * @c with_*_parameters function pointers to constrain parameters, then call
   * @c and_return_*_value to configure the return value.
   */
  struct MutinyMockExpectedCall
  {
    /**
     * @brief Constrain a bool parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_bool_parameters)(
        const char* name,
        int value
    );

    /**
     * @brief Constrain an int parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_int_parameters)(
        const char* name,
        int value
    );

    /**
     * @brief Constrain an unsigned int parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_unsigned_int_parameters)(
        const char* name,
        unsigned int value
    );

    /**
     * @brief Constrain a long int parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_long_int_parameters)(
        const char* name,
        long int value
    );

    /**
     * @brief Constrain an unsigned long int parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_unsigned_long_int_parameters)(
        const char* name,
        unsigned long int value
    );

    /**
     * @brief Constrain a long long parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_long_long_int_parameters)(
        const char* name,
        long long value
    );

    /**
     * @brief Constrain an unsigned long long parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_unsigned_long_long_int_parameters)(
        const char* name,
        unsigned long long value
    );

    /**
     * @brief Constrain a double parameter (exact).
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_double_parameters)(
        const char* name,
        double value
    );

    /**
     * @brief Constrain a double parameter with tolerance.
     * @param name Name.
     * @param value Value.
     * @param tolerance Max difference.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_double_parameters_and_tolerance)(
        const char* name,
        double value,
        double tolerance
    );

    /**
     * @brief Constrain a string parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_string_parameters)(
        const char* name,
        const char* value
    );

    /**
     * @brief Constrain a void* parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_pointer_parameters)(
        const char* name,
        void* value
    );

    /**
     * @brief Constrain a const void* parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_const_pointer_parameters)(
        const char* name,
        const void* value
    );

    /**
     * @brief Constrain a function pointer parameter.
     * @param name Name.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_function_pointer_parameters)(
        const char* name,
        void (*value)(void)
    );

    /**
     * @brief Constrain a memory buffer parameter.
     * @param name Name.
     * @param value Buffer.
     * @param size Size.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_memory_buffer_parameter)(
        const char* name,
        const unsigned char* value,
        size_t size
    );

    /**
     * @brief Constrain a custom-type object parameter.
     * @param type Type name.
     * @param name Name.
     * @param value Object pointer.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_parameter_of_type)(
        const char* type,
        const char* name,
        const void* value
    );

    /**
     * @brief Configure an output parameter (plain @c memcpy).
     * @param name Name.
     * @param value Source data.
     * @param size Bytes to copy.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_output_parameter_returning)(
        const char* name,
        const void* value,
        size_t size
    );

    /**
     * @brief Configure a custom-type output parameter.
     * @param type Type name.
     * @param name Name.
     * @param value Source object.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_output_parameter_of_type_returning)(
        const char* type,
        const char* name,
        const void* value
    );

    /**
     * @brief Expect an output parameter to be passed but leave the buffer
     * unchanged.
     * @param name Name.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_unmodified_output_parameter)(
        const char* name
    );

    /**
     * @brief Capture a parameter (plain @c memcpy).
     * @param name Name.
     * @param value Buffer to copy the actual value into.
     * @param size Bytes to copy.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_captured_parameter)(
        const char* name,
        void* value,
        size_t size
    );

    /**
     * @brief Capture a parameter of a custom object type.
     * @param type Type name.
     * @param name Name.
     * @param value Buffer to copy the actual object into.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*with_captured_parameter_of_type)(
        const char* type,
        const char* name,
        void* value
    );

    /**
     * @brief Allow the actual call to pass extra unlisted parameters.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*ignore_other_parameters)(void);

    /**
     * @brief Configure a bool return value.
     * @param value Value to return.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_bool_value)(int value);

    /**
     * @brief Configure an unsigned int return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_unsigned_int_value)(
        unsigned int value
    );

    /**
     * @brief Configure an int return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_int_value)(int value);

    /**
     * @brief Configure a long int return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_long_int_value)(long int value);

    /**
     * @brief Configure an unsigned long int return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_unsigned_long_int_value)(
        unsigned long int value
    );

    /**
     * @brief Configure a <tt>long long</tt> return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_long_long_int_value)(
        long long value
    );

    /**
     * @brief Configure an <tt>unsigned long long</tt> return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_unsigned_long_long_int_value)(
        unsigned long long value
    );

    /**
     * @brief Configure a double return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_double_value)(double value);

    /**
     * @brief Configure a string return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_string_value)(
        const char* value
    );

    /**
     * @brief Configure a <tt>void*</tt> return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_pointer_value)(void* value);

    /**
     * @brief Configure a <tt>const void*</tt> return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_const_pointer_value)(
        const void* value
    );

    /**
     * @brief Configure a function pointer return value.
     * @param value Value.
     * @return this.
     */
    struct MutinyMockExpectedCall* (*and_return_function_pointer_value)(
        void (*value)(void)
    );
  };

  /** Function type for a custom equality predicate (C comparator). */
  typedef int (*MutinyMockTypeEqualFunction)(
      const void* object1,
      const void* object2
  );

  /** Function type for converting a custom object to a display string. */
  typedef const char* (*MutinyMockTypeValueToStringFunction)(
      const void* object1
  );

  /** Function type for copying a custom object. */
  typedef void (*MutinyMockTypeCopyFunction)(void* dst, const void* src);

  /**
   * @brief Vtable for the global or named mock support scope (C interface).
   *
   * Obtained by calling @ref mutiny_mock() or @ref mutiny_mock_scope(). Use the
   * function pointers to set up expectations, report actual calls, install
   * comparators, and verify/clear state.
   *
   * @see mutiny_mock(), mutiny_mock_scope()
   */
  struct MutinyMockSupport
  {
    /** @brief Require calls to arrive in registration order. */
    void (*strict_order)(void);

    /**
     * @brief Expect exactly one call.
     * @param name Function name.
     * @return Expected-call vtable.
     */
    struct MutinyMockExpectedCall* (*expect_one_call)(const char* name);

    /**
     * @brief Assert function is never called.
     * @param name Function name.
     */
    void (*expect_no_call)(const char* name);

    /**
     * @brief Expect exactly @p number calls.
     * @param number Count.
     * @param name Function name.
     * @return Expected-call vtable.
     */
    struct MutinyMockExpectedCall* (*expect_n_calls)(
        unsigned int number,
        const char* name
    );

    /**
     * @brief Report an actual call from a mock implementation.
     * @param name Function name.
     * @return Actual-call vtable.
     */
    struct MutinyMockActualCall* (*actual_call)(const char* name);

    /** @return Non-zero if the last actual call has a configured return value.
     */
    int (*has_return_value)(void);

    /** @return The last actual call's return value as a tagged union. */
    struct MutinyMockValue (*return_value)(void);

    /** @return The configured bool return value (as <tt>int</tt>). */
    int (*bool_return_value)(void);

    /**
     * @return Bool return value or @p default_value.
     * @param default_value Fallback.
     */
    int (*return_bool_value_or_default)(int default_value);

    /** @return The configured int return value. */
    int (*int_return_value)(void);

    /**
     * @return Int return value or @p default_value.
     * @param default_value Fallback.
     */
    int (*return_int_value_or_default)(int default_value);

    /** @return The configured <tt>unsigned int</tt> return value. */
    unsigned int (*unsigned_int_return_value)(void);

    /**
     * @return Unsigned int return value or @p default_value.
     * @param default_value Fallback.
     */
    unsigned int (*return_unsigned_int_value_or_default)(
        unsigned int default_value
    );

    /** @return The configured <tt>long int</tt> return value. */
    long int (*long_int_return_value)(void);

    /**
     * @return Long int return value or @p default_value.
     * @param default_value Fallback.
     */
    long int (*return_long_int_value_or_default)(long int default_value);

    /** @return The configured <tt>unsigned long int</tt> return value. */
    unsigned long int (*unsigned_long_int_return_value)(void);

    /** @return <tt>unsigned long int</tt> return value or @p default_value.
     * @param default_value Fallback.
     */
    unsigned long int (*return_unsigned_long_int_value_or_default)(
        unsigned long int default_value
    );

    /** @return The configured <tt>long long</tt> return value. */
    long long (*long_long_int_return_value)(void);

    /**
     * @return <tt>long long</tt> return value or @p default_value.
     * @param default_value Fallback.
     */
    long long (*return_long_long_int_value_or_default)(long long default_value);

    /** @return The configured <tt>unsigned long long</tt> return value. */
    unsigned long long (*unsigned_long_long_int_return_value)(void);

    /**
     * @return <tt>unsigned long long</tt> return value or @p default_value.
     * @param default_value Fallback.
     */
    unsigned long long (*return_unsigned_long_long_int_value_or_default)(
        unsigned long long default_value
    );

    /** @return The configured C string return value. */
    const char* (*string_return_value)(void);

    /**
     * @return String return value or @p default_value.
     * @param default_value Fallback.
     */
    const char* (*return_string_value_or_default)(const char* default_value);

    /** @return The configured <tt>double</tt> return value. */
    double (*double_return_value)(void);

    /**
     * @return <tt>double</tt> return value or @p default_value.
     * @param default_value Fallback.
     */
    double (*return_double_value_or_default)(double default_value);

    /** @return The configured <tt>void*</tt> return value. */
    void* (*pointer_return_value)(void);

    /**
     * @return Pointer return value or @p default_value.
     * @param default_value Fallback.
     */
    void* (*return_pointer_value_or_default)(void* default_value);

    /** @return The configured <tt>const void*</tt> return value. */
    const void* (*const_pointer_return_value)(void);

    /**
     * @return <tt>const</tt> pointer return value or @p default_value.
     * @param default_value Fallback.
     */
    const void* (*return_const_pointer_value_or_default)(
        const void* default_value
    );

    /** @return The configured function pointer return value. */
    void (*(*function_pointer_return_value)(void))(void);

    /**
     * @return Function pointer return value or @p default_value.
     * @param default_value Fallback.
     */
    void (*(*return_function_pointer_value_or_default)(void (*default_value)(
        void
    )))(void);

    /**
     * @brief Store a bool data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_bool_data)(const char* name, int value);

    /**
     * @brief Store an int data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_int_data)(const char* name, int value);

    /**
     * @brief Store an <tt>unsigned int</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_unsigned_int_data)(const char* name, unsigned int value);

    /**
     * @brief Store a long int data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_long_int_data)(const char* name, long int value);

    /**
     * @brief Store an <tt>unsigned long int</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_unsigned_long_int_data)(
        const char* name,
        unsigned long int value
    );

    /**
     * @brief Store a <tt>long long int</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_long_long_int_data)(const char* name, long long value);

    /**
     * @brief Store an <tt>unsigned long long int</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_unsigned_long_long_int_data)(
        const char* name,
        unsigned long long value
    );

    /**
     * @brief Store a string data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_string_data)(const char* name, const char* value);

    /**
     * @brief Store a <tt>double</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_double_data)(const char* name, double value);

    /**
     * @brief Store a <tt>void*</tt> data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_pointer_data)(const char* name, void* value);

    /**
     * @brief Store a `const void*` data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_const_pointer_data)(const char* name, const void* value);

    /**
     * @brief Store a function pointer data value.
     * @param name Key.
     * @param value Value.
     */
    void (*set_function_pointer_data)(const char* name, void (*value)(void));

    /**
     * @brief Store a mutable object pointer with type name.
     * @param name Key.
     * @param type Type name.
     * @param value Object pointer.
     */
    void (*set_data_object)(const char* name, const char* type, void* value);

    /**
     * @brief Store a <tt>const</tt> object pointer with type name.
     * @param name Key.
     * @param type Type name.
     * @param value Object pointer.
     */
    void (*set_data_const_object)(
        const char* name,
        const char* type,
        const void* value
    );

    /**
     * @return Previously stored data value.
     * @param name Key.
     */
    struct MutinyMockValue (*get_data)(const char* name);

    /** @brief Disable this scope; @ref actual_call() becomes a no-op. */
    void (*disable)(void);

    /** @brief Re-enable this scope after Ref disable(). */
    void (*enable)(void);

    /**
     * @brief Suppress unexpected-call failures.
     *
     * All @ref actual_call() invocations are silently accepted without matching
     * an expectation.
     */
    void (*ignore_other_calls)(void);

    /**
     * @brief Verify all expected calls were fulfilled; fail the test if not.
     *
     * Typically called at the end of each test.
     */
    void (*check_expectations)(void);

    /** @return Non-zero if there are unfulfilled expectations remaining. */
    int (*expected_calls_left)(void);

    /**
     * @brief Remove all expectations, actual calls, and data.
     *
     * Call after @ref check_expectations() to leave a clean state.
     */
    void (*clear)(void);
    /**
     * @brief Choose whether mock failures crash the process.
     *
     * @param should_crash  Non-zero to crash on failure (useful with a
     * debugger).
     */
    void (*crash_on_failure)(unsigned should_crash);

    /**
     * @brief Register a C comparator for objects of type @p type_name.
     *
     * @param type_name      Type name key.
     * @param is_equal       Equality predicate function.
     * @param value_to_string  String-conversion function.
     */
    void (*install_comparator)(
        const char* type_name,
        MutinyMockTypeEqualFunction is_equal,
        MutinyMockTypeValueToStringFunction value_to_string
    );

    /**
     * @brief Register a C copier for objects of type @p type_name.
     *
     * @param type_name  Type name key.
     * @param copier     Copy function.
     */
    void (*install_copier)(
        const char* type_name,
        MutinyMockTypeCopyFunction copier
    );

    /** @brief Remove all installed comparators and copiers from this scope. */
    void (*remove_all_comparators_and_copiers)(void);
  };

  /**
   * @brief Access the global mock support scope.
   *
   * @return Pointer to the @ref MutinyMockSupport vtable for the global scope.
   *
   * @see mutiny_mock_scope()
   */
  MUTINY_EXPORT struct MutinyMockSupport* mutiny_mock(void);

  /**
   * @brief Access a named mock support scope.
   *
   * @param scope  Scope name string.
   * @return Pointer to the @ref MutinyMockSupport vtable for the named scope.
   *
   * @see mutiny_mock()
   */
  MUTINY_EXPORT struct MutinyMockSupport* mutiny_mock_scope(const char* scope);

#ifdef __cplusplus
}
#endif

#endif
