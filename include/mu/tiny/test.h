/**
 * @file
 * @brief C interface for writing mutiny tests in pure C.
 *
 * Include this header in C test files. Test definitions use the same macro
 * names as the C++ interface (@ref TEST, @ref SKIPPED_TEST, @ref
 * XFAIL_TEST) but expand to C function stubs that are called by
 * corresponding C++ wrapper macros in the bridging C++ file.
 *
 * Assertion macros use type-specific names (@ref CHECK_EQUAL_INT, @ref
 * CHECK_EQUAL_LONG, etc.) because C has no function overloading. @ref
 * CHECK_EQUAL provides a type-generic alternative via C11 `_Generic`.
 */

#ifndef INCLUDED_MU_TINY_TEST_H
#define INCLUDED_MU_TINY_TEST_H

#include "mu/tiny/export.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else

/**
 * @brief Fail if @p expected != @p actual (bool).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_BOOL(expected, actual)                                     \
  mutiny_check_equal_bool(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_BOOL with a custom failure message. */
#define CHECK_EQUAL_BOOL_TEXT(expected, actual, text)                          \
  mutiny_check_equal_bool(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (int).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_INT(expected, actual)                                      \
  mutiny_check_equal_int(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_INT with a custom failure message. */
#define CHECK_EQUAL_INT_TEXT(expected, actual, text)                           \
  mutiny_check_equal_int(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (unsigned int).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_UINT(expected, actual)                                     \
  mutiny_check_equal_uint(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_UINT with a custom failure message. */
#define CHECK_EQUAL_UINT_TEXT(expected, actual, text)                          \
  mutiny_check_equal_uint(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (long).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_LONG(expected, actual)                                     \
  mutiny_check_equal_long(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_LONG with a custom failure message. */
#define CHECK_EQUAL_LONG_TEXT(expected, actual, text)                          \
  mutiny_check_equal_long(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (unsigned long).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_ULONG(expected, actual)                                    \
  mutiny_check_equal_ulong(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_ULONG with a custom failure message. */
#define CHECK_EQUAL_ULONG_TEXT(expected, actual, text)                         \
  mutiny_check_equal_ulong(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (long long).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_LONGLONG(expected, actual)                                 \
  mutiny_check_equal_longlong(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_LONGLONG with a custom failure message. */
#define CHECK_EQUAL_LONGLONG_TEXT(expected, actual, text)                      \
  mutiny_check_equal_longlong(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (unsigned long long).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_ULONGLONG(expected, actual)                                \
  mutiny_check_equal_ulonglong(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_ULONGLONG with a custom failure message. */
#define CHECK_EQUAL_ULONGLONG_TEXT(expected, actual, text)                     \
  mutiny_check_equal_ulonglong(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected and @p actual differ by more than @p threshold
 * (double).
 * @param expected   Expected value.
 * @param actual     Actual value.
 * @param threshold  Maximum allowed absolute difference.
 */
#define CHECK_EQUAL_DOUBLE(expected, actual, threshold)                        \
  mutiny_check_equal_double(expected, actual, threshold, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_DOUBLE with a custom failure message. */
#define CHECK_EQUAL_DOUBLE_TEXT(expected, actual, threshold, text)             \
  mutiny_check_equal_double(                                                   \
      expected, actual, threshold, text, __FILE__, __LINE__                    \
  )

/**
 * @brief Fail if @p expected != @p actual (char).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_CHAR(expected, actual)                                     \
  mutiny_check_equal_char(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_CHAR with a custom failure message. */
#define CHECK_EQUAL_CHAR_TEXT(expected, actual, text)                          \
  mutiny_check_equal_char(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (unsigned char / byte).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_UBYTE(expected, actual)                                    \
  mutiny_check_equal_ubyte(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_UBYTE with a custom failure message. */
#define CHECK_EQUAL_UBYTE_TEXT(expected, actual, text)                         \
  mutiny_check_equal_ubyte(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p expected != @p actual (signed char).
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL_SBYTE(expected, actual)                                    \
  mutiny_check_equal_sbyte(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_SBYTE with a custom failure message. */
#define CHECK_EQUAL_SBYTE_TEXT(expected, actual, text)                         \
  mutiny_check_equal_sbyte(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if the C strings @p expected and @p actual differ (strcmp).
 * @param expected  Expected C string.
 * @param actual    Actual C string.
 */
#define CHECK_EQUAL_STRING(expected, actual)                                   \
  mutiny_check_equal_string(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_STRING with a custom failure message. */
#define CHECK_EQUAL_STRING_TEXT(expected, actual, text)                        \
  mutiny_check_equal_string(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if pointer values @p expected and @p actual differ.
 * @param expected  Expected pointer.
 * @param actual    Actual pointer.
 */
#define CHECK_EQUAL_POINTER(expected, actual)                                  \
  mutiny_check_equal_pointer(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_POINTER with a custom failure message. */
#define CHECK_EQUAL_POINTER_TEXT(expected, actual, text)                       \
  mutiny_check_equal_pointer(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Fail if @p size bytes at @p expected and @p actual differ (memcmp).
 * @param expected  Pointer to expected memory region.
 * @param actual    Pointer to actual memory region.
 * @param size      Number of bytes to compare.
 */
#define CHECK_EQUAL_MEMCMP(expected, actual, size)                             \
  mutiny_check_equal_memcmp(expected, actual, size, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL_MEMCMP with a custom failure message. */
#define CHECK_EQUAL_MEMCMP_TEXT(expected, actual, size, text)                  \
  mutiny_check_equal_memcmp(expected, actual, size, text, __FILE__, __LINE__)

/** @brief Type dispatch used by @ref CHECK_EQUAL / @ref CHECK_EQUAL_TEXT. */
#define MUTINY_CHECK_EQUAL_GENERIC(actual)                                     \
  _Generic(                                                                    \
      (actual),                                                                \
      bool: mutiny_check_equal_bool,                                           \
      int: mutiny_check_equal_int,                                             \
      unsigned int: mutiny_check_equal_uint,                                   \
      long: mutiny_check_equal_long,                                           \
      unsigned long: mutiny_check_equal_ulong,                                 \
      long long: mutiny_check_equal_longlong,                                  \
      unsigned long long: mutiny_check_equal_ulonglong,                        \
      char: mutiny_check_equal_char,                                           \
      unsigned char: mutiny_check_equal_ubyte,                                 \
      signed char: mutiny_check_equal_sbyte,                                   \
      char*: mutiny_check_equal_string,                                        \
      const char*: mutiny_check_equal_string,                                  \
      default: mutiny_check_equal_pointer                                      \
  )

/**
 * @brief Fail if @p expected != @p actual.
 *
 * Type-generic equivalent of the ``CHECK_EQUAL_*`` family (@ref
 * CHECK_EQUAL_BOOL, @ref CHECK_EQUAL_INT, @ref CHECK_EQUAL_UINT, @ref
 * CHECK_EQUAL_LONG, @ref CHECK_EQUAL_ULONG, @ref CHECK_EQUAL_LONGLONG, @ref
 * CHECK_EQUAL_ULONGLONG, @ref CHECK_EQUAL_CHAR, @ref CHECK_EQUAL_UBYTE, @ref
 * CHECK_EQUAL_SBYTE, @ref CHECK_EQUAL_STRING, @ref CHECK_EQUAL_POINTER),
 * dispatching on the type of @p actual via C11 `_Generic`.
 *
 * Not available for `double` (needs a threshold, see @ref
 * CHECK_EQUAL_DOUBLE) or buffers (see @ref CHECK_EQUAL_MEMCMP), since those
 * take a different number of arguments.
 *
 * @param expected  Expected value.
 * @param actual    Actual value.
 */
#define CHECK_EQUAL(expected, actual)                                          \
  MUTINY_CHECK_EQUAL_GENERIC(actual)(expected, actual, "", __FILE__, __LINE__)

/** @brief @ref CHECK_EQUAL with a custom failure message. */
#define CHECK_EQUAL_TEXT(expected, actual, text)                               \
  MUTINY_CHECK_EQUAL_GENERIC(actual)(expected, actual, text, __FILE__, __LINE__)

/**
 * @brief Unconditionally fail with a message.
 * @param text  Human-readable failure message.
 */
#define FAIL_TEST(text) mutiny_fail(text, __FILE__, __LINE__)

/**
 * @brief Fail if @p condition is zero (false).
 * @param condition  Integer condition to test.
 */
#define CHECK(condition)                                                       \
  mutiny_check(condition, #condition, "", __FILE__, __LINE__)

/** @brief @ref CHECK with a custom failure message. */
#define CHECK_TEXT(condition, text)                                            \
  mutiny_check(condition, #condition, text, __FILE__, __LINE__)

/**
 * @brief Attach a key/value property to the currently running test.
 *
 * Properties appear in JUnit XML output. This macro is available in both
 * C and C++ test files.
 *
 * @param name   Property name.
 * @param value  Property value.
 */
#define TEST_PROPERTY(name, value) mutiny_add_test_property_c((name), (value))

/**
 * @brief Define the C setup function for a group declared with
 * @ref TEST_GROUP_C_WRAPPER.
 *
 * The body you write runs before each test in the group.
 *
 * @param group_name  Must match the @ref TEST_GROUP_C_WRAPPER declaration.
 */
#define TEST_GROUP_SETUP(group_name)                                           \
  extern void group_##group_name##_setup_wrapper_c(void);                      \
  void group_##group_name##_setup_wrapper_c(void)

/**
 * @brief Define the C teardown function for a group declared with
 * @ref TEST_GROUP_C_WRAPPER.
 *
 * @param group_name  Must match the @ref TEST_GROUP_C_WRAPPER declaration.
 */
#define TEST_GROUP_TEARDOWN(group_name)                                        \
  extern void group_##group_name##_teardown_wrapper_c(void);                   \
  void group_##group_name##_teardown_wrapper_c(void)

/**
 * @brief Define a C test function body.
 *
 * The function stub is called by the corresponding @ref TEST_C_WRAPPER macro in
 * the C++ bridge file.
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define TEST(group_name, test_name)                                            \
  extern void test_##group_name##_##test_name##_wrapper_c(void);               \
  void test_##group_name##_##test_name##_wrapper_c(void)

/**
 * @brief Define a C ignored-test function body (body is not executed).
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define SKIPPED_TEST(group_name, test_name)                                    \
  extern void ignore_##group_name##_##test_name##_wrapper_c(void);             \
  void ignore_##group_name##_##test_name##_wrapper_c(void)

/**
 * @brief Define a C expected-to-fail test function body.
 *
 * @param group_name  Test group.
 * @param test_name   Test name.
 */
#define XFAIL_TEST(group_name, test_name)                                      \
  extern void expect_fail_##group_name##_##test_name##_wrapper_c(void);        \
  void expect_fail_##group_name##_##test_name##_wrapper_c(void)
#endif

  /**
   * @brief C implementation of @ref CHECK_EQUAL_BOOL.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_bool(
      bool expected,
      bool actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_INT.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_int(
      int expected,
      int actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_UINT.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_uint(
      unsigned int expected,
      unsigned int actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_LONG.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_long(
      long expected,
      long actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_ULONG.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_ulong(
      unsigned long expected,
      unsigned long actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_LONGLONG.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_longlong(
      long long expected,
      long long actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_ULONGLONG.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_ulonglong(
      unsigned long long expected,
      unsigned long long actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_DOUBLE.
   * @param expected Expected.
   * @param actual Actual.
   * @param threshold Threshold.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_double(
      double expected,
      double actual,
      double threshold,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_CHAR.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_char(
      char expected,
      char actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_UBYTE.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_ubyte(
      unsigned char expected,
      unsigned char actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_SBYTE.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_sbyte(
      signed char expected,
      signed char actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_STRING.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_string(
      const char* expected,
      const char* actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_POINTER.
   * @param expected Expected.
   * @param actual Actual.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_pointer(
      const void* expected,
      const void* actual,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK_EQUAL_MEMCMP.
   * @param expected Expected buffer.
   * @param actual Actual buffer.
   * @param size Bytes to compare.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check_equal_memcmp(
      const void* expected,
      const void* actual,
      size_t size,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref FAIL_TEST.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_fail(
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation of @ref CHECK / @ref CHECK_TEXT.
   * @param condition Condition (non-zero = true).
   * @param condition_string Stringified condition.
   * @param text Message.
   * @param file_name File.
   * @param line_number Line.
   */
  MUTINY_EXPORT void mutiny_check(
      int condition,
      const char* condition_string,
      const char* text,
      const char* file_name,
      int_least32_t line_number
  );

  /**
   * @brief C implementation backing @ref TEST_PROPERTY in C files.
   * @param name Property name.
   * @param value Property value.
   */
  MUTINY_EXPORT void mutiny_add_test_property(
      const char* name,
      const char* value
  );

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_MU_TINY_TEST_H */
