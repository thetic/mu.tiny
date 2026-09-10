/**
 * @file
 * @brief Lightweight string class and string-conversion utilities for the
 *        mutiny test framework.
 *
 * When @ref MUTINY_USE_STD_STRING is defined, @ref mu::tiny::String is an
 * alias for `std::string` and the custom class body is omitted entirely.
 * Otherwise a self-contained `String` class is provided that avoids any
 * dependency on the C++ standard library, allowing the framework to be used
 * in bare-metal and other constrained environments.
 *
 * The free functions below (@ref mu::tiny::string_from,
 * @ref mu::tiny::hex_string_from,
 * @ref mu::tiny::brackets_formatted_hex_string_from, and related utilities)
 * are used
 * throughout the framework to convert arbitrary values to their printable
 * representations in assertion failure messages.
 */

#ifndef INCLUDED_MU_TINY_STRING_HPP
#define INCLUDED_MU_TINY_STRING_HPP

#include "mu/tiny/export.h"
#include "mu/tiny/features.hpp"

#include <stdarg.h>
#include <stddef.h>

#if MUTINY_USE_STD_CPP_LIB
#include <string>
#include <type_traits>
#endif

#ifndef __has_attribute
#define MUTINY_HAS_ATTRIBUTE(x) 0
#else
#define MUTINY_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif

#if defined(__MINGW32__) && defined(__USE_MINGW_ANSI_STDIO) &&                 \
    (__USE_MINGW_ANSI_STDIO > 0)
#define MUTINY_CHECK_FORMAT_TYPE __MINGW_PRINTF_FORMAT
#else
#define MUTINY_CHECK_FORMAT_TYPE printf
#endif

namespace mu {
namespace tiny {

class StringView;

#if MUTINY_USE_STD_STRING
/**
 * @brief String type used throughout the mutiny framework.
 *
 * When @ref MUTINY_USE_STD_STRING is enabled this is an alias for
 * `std::string`; otherwise it is the custom class defined below.
 */
using String = std::string;
#else
/**
 * @brief Minimal string class used throughout the mutiny framework.
 *
 * Provides just enough string functionality for assertion messages and
 * internal bookkeeping without depending on the C++ standard library.
 * The interface deliberately mirrors the subset of `std::string` that
 * the framework uses so that switching to `std::string` via
 * @ref MUTINY_USE_STD_STRING requires no call-site changes.
 */
class MUTINY_EXPORT String
{
  friend bool operator==(const String& left, const String& right);
  friend bool operator!=(const String& left, const String& right);

public:
  /** @brief Construct from a NUL-terminated C string (default: empty). */
  String(const char* value = "");
  /** @brief Construct from a pointer and an explicit byte count. */
  String(const char* value, size_t len);
  /** @brief Construct a copy from a @ref StringView (explicit; allocates). */
  explicit String(StringView value);
  /**
   * @brief Construct a string of @p count copies of character @p ch.
   *
   * @param count  Number of characters.
   * @param ch     Character to fill with.
   */
  String(size_t count, char ch);
  /** @brief Copy constructor. */
  String(const String& other);
  /** @brief Move constructor. */
  String(String&& other) noexcept;
  ~String();

  /** @brief Copy-assignment operator. */
  String& operator=(const String& other);
  /** @brief Move-assignment operator. */
  String& operator=(String&& other) noexcept;
  /** @brief Return the concatenation of this string and @p rhs. */
  String operator+(const String& rhs) const;
  /** @brief Append @p rhs and return a reference to this string. */
  String& operator+=(const String& rhs);
  /** @brief Append a C string and return a reference to this string. */
  String& operator+=(const char* rhs);
  /** @brief Append a single character and return a reference to this string. */
  String& operator+=(char ch);
  /** @brief Append the contents of a @ref StringView. */
  String& operator+=(StringView sv);

  /** @brief Sentinel value returned by `find()` when no match is found. */
  static constexpr size_t npos = static_cast<size_t>(-1);

  /** @return Reference to the character at position @p pos. */
  char& operator[](size_t pos) { return data()[pos]; }
  /** @return Const reference to the character at position @p pos. */
  const char& operator[](size_t pos) const { return c_str()[pos]; }

  /**
   * @brief Find the first occurrence of @p ch at or after @p pos.
   *
   * @param ch   Character to search for.
   * @param pos  Starting position for the search.
   * @return Index of the first match, or `npos` if not found.
   */
  size_t find(char ch, size_t pos = 0) const;
  /**
   * @brief Find the first occurrence of the substring @p s at or after @p pos.
   *
   * @param s    Substring to search for.
   * @param pos  Starting position for the search.
   * @return Index of the first match, or `npos` if not found.
   */
  size_t find(const char* s, size_t pos = 0) const;

  /** @return Substring from @p begin_pos to end-of-string. */
  String substr(size_t begin_pos) const;
  /**
   * @return Substring of at most @p amount characters starting at
   *         @p begin_pos.
   */
  String substr(size_t begin_pos, size_t amount) const;

  /** @return NUL-terminated pointer to the string's character data. */
  const char* c_str() const;
  /** @return Read-only pointer to the character data. */
  const char* data() const;
  /** @return Writable pointer to the character data. */
  char* data();
  /** @return Number of characters (not counting the NUL terminator). */
  size_t size() const;
  /** @return Number of characters; synonym for `size()`. */
  size_t length() const { return size(); }

  /** @return Number of characters that fit without reallocation. */
  size_t capacity() const
  {
    return (buffer_size_ != 0U) ? buffer_size_ - 1 : 0;
  }

  /** @return true if the string has zero characters. */
  bool empty() const;
  /** @brief Reset the string to empty without releasing the buffer. */
  void clear();
  /**
   * @brief Ensure at least @p new_capacity characters fit without
   *        reallocation.
   */
  void reserve(size_t new_capacity);
  /**
   * @brief Resize the string to exactly @p new_size characters, padding
   *        with NUL bytes if growing.
   */
  void resize(size_t new_size);

  friend bool operator<(const String& left, const String& right);

private:
  void deallocate_internal_buffer();
  void set_internal_buffer_as_empty_string();
  void copy_buffer_to_new_internal_buffer(const char* other_buffer);
  void copy_buffer_to_new_internal_buffer(
      const char* other_buffer,
      size_t buffer_size
  );
  void copy_buffer_to_new_internal_buffer(const String& other_buffer);

  char* buffer_;
  size_t buffer_size_;
  size_t size_;
};
#endif

/** @brief Return true if @p str contains @p substr. */
MUTINY_EXPORT bool string_contains(const String& str, const String& substr);
/** @brief Return true if @p str starts with @p prefix. */
MUTINY_EXPORT bool string_starts_with(const String& str, const String& prefix);
/** @brief Return true if @p str ends with @p suffix. */
MUTINY_EXPORT bool string_ends_with(const String& str, const String& suffix);

/** @brief Replace every @p from character in @p str with @p to. */
MUTINY_EXPORT void string_replace(String& str, char from, char to);
/**
 * @brief Replace every occurrence of the substring @p from in @p str with
 *        @p to.
 */
MUTINY_EXPORT void string_replace(
    String& str,
    const char* from,
    const char* to
);

/**
 * @brief Wrappers for C standard library functions, placed in the
 *        @ref mu::tiny::test namespace to avoid polluting the global namespace
 *        when the standard library is unavailable.
 */
MUTINY_EXPORT bool iscntrl(char ch);
/** @brief Return the length of the NUL-terminated string @p str. */
MUTINY_EXPORT size_t strlen(const char* str);
/** @brief Return a pointer to the first @p s2 in @p s1, or null. */
MUTINY_EXPORT const char* strstr(const char* s1, const char* s2);

/** @brief Parse @p str as an unsigned long integer. */
MUTINY_EXPORT unsigned long strtoul(const char* str);
/** @brief Compare @p s1 and @p s2; return negative, zero, or positive. */
MUTINY_EXPORT int strcmp(const char* s1, const char* s2);
/** @brief Compare at most @p n characters of @p s1 and @p s2. */
MUTINY_EXPORT int strncmp(const char* s1, const char* s2, size_t n);
/** @brief Convert @p ch to lowercase. */
MUTINY_EXPORT char tolower(char ch);
/** @brief Compare @p n bytes of @p s1 and @p s2; return <0, 0, or >0. */
MUTINY_EXPORT int memcmp(const void* s1, const void* s2, size_t n);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(bool value);

/** @brief Return the pointer address of @p value as a hex string. */
MUTINY_EXPORT String string_from(const void* value);

/** @brief Return the function pointer address of @p value as a hex string. */
MUTINY_EXPORT String string_from(void (*value)());

/** @brief Route any typed function pointer to the @c void(*)() overload. */
template<typename RET, typename... ARGS>
inline String string_from(RET (*value)(ARGS...))
{
  return string_from(reinterpret_cast<void (*)()>(value));
}

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(char value);

/** @brief Return @p value, or "(null)" if @p value is null. */
MUTINY_EXPORT String string_from(const char* value);

/** @brief Return @p value as a string, or "(null)" if @p value is null. */
MUTINY_EXPORT String string_from_or_null(const char* value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(int value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(unsigned int value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT inline String string_from(signed char value)
{
  return string_from(static_cast<int>(value));
}

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT inline String string_from(unsigned char value)
{
  return string_from(static_cast<unsigned int>(value));
}

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(long value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(unsigned long value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(long long value);

/** @brief Return the decimal string representation of @p value. */
MUTINY_EXPORT String string_from(unsigned long long value);

constexpr int default_precision{ 6 };

/**
 * @brief Return the decimal string representation of @p value with the given
 *        @p precision.
 */
MUTINY_EXPORT String
string_from(float value, int precision = default_precision);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(unsigned int value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(int value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(signed char value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(long value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(unsigned long value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(long long value);

/** @brief Return @p value as a `0x`-prefixed hexadecimal string. */
MUTINY_EXPORT String hex_string_from(unsigned long long value);

/** @brief Return the address of @p value as a `0x`-prefixed hex string. */
MUTINY_EXPORT String hex_string_from(const void* value);

/** @brief Return the address of @p value as a `0x`-prefixed hex string. */
MUTINY_EXPORT String hex_string_from(void (*value)());

/**
 * @brief Return the decimal string representation of @p value with the given
 *        @p precision.
 */
MUTINY_EXPORT String
string_from(double value, int precision = default_precision);

/** @brief Return a copy of @p other as a String. */
MUTINY_EXPORT String string_from(const String& other);

/** @brief Construct a String from a @ref StringView. */
MUTINY_EXPORT String string_from(StringView value);

/** @brief Return the string representation of a null pointer constant. */
MUTINY_EXPORT String string_from(decltype(nullptr) value);

#if MUTINY_USE_STD_CPP_LIB

#if !MUTINY_USE_STD_STRING
/** @brief Convert a `std::string` to a `String`. */
MUTINY_EXPORT String string_from(std::string const& str);
#endif
#endif

/** @brief Route a non-const void pointer to the `const void*` overload. */
MUTINY_EXPORT inline String string_from(void* value)
{
  return string_from(static_cast<const void*>(value));
}

// Catch any pointer type that lacks its own overload and route to const void*.
// This template is more specialised than the fallback below, so it wins
// in partial ordering when the argument is a non-void pointer.
template<typename T>
inline String string_from(T* value)
{
  return string_from(static_cast<const void*>(value));
}

// Fallback: fires only for class types (structs/classes) that have no
// string_from() overload.  Enums and arithmetic types are excluded so they
// continue to use their existing implicit-conversion overloads (e.g. enum→int).
// When the standard library is available we use std::is_class to express this
// constraint via SFINAE; without it the fallback is omitted and a missing
// overload surfaces as a linker error instead.
#if MUTINY_USE_STD_CPP_LIB
template<
    typename T,
    typename std::enable_if<std::is_class<T>::value, int>::type = 0>
String string_from(const T& /*val*/)
{
  static_assert(
      sizeof(T) == 0,
      "No mu::tiny::string_from() overload for this type. "
      "Provide: mu::tiny::String mu::tiny::string_from(const "
      "YourType&);"
  );
  return String();
}
#endif

#if MUTINY_HAS_ATTRIBUTE(format)
#define MUTINY_CHECK_FORMAT(type, format_parameter, other_parameters)          \
  __attribute__((format(type, format_parameter, other_parameters)))
#else
#define MUTINY_CHECK_FORMAT(                                                   \
    type, format_parameter, other_parameters                                   \
) /* type, format_parameter, other_parameters */
#endif
/**
 * @brief Format a string using `printf`-style @p format and variadic
 *        arguments.
 */
MUTINY_EXPORT String string_from_format(
    const char* format,
    ...
) MUTINY_CHECK_FORMAT(MUTINY_CHECK_FORMAT_TYPE, 1, 2);

/** @brief Format a string using `printf`-style @p format and a `va_list`. */
MUTINY_EXPORT String v_string_from_format(const char* format, va_list args);

/** @brief Return a hex dump of @p size bytes starting at @p value. */
MUTINY_EXPORT String
string_from_binary(const unsigned char* value, size_t size);

/** @brief Return a hex dump of @p size bytes, or "(null)" if null. */
MUTINY_EXPORT String
string_from_binary_or_null(const unsigned char* value, size_t size);

/** @brief Return a hex dump of @p size bytes prefixed with the byte count. */
MUTINY_EXPORT String
string_from_binary_with_size(const unsigned char* value, size_t size);

/**
 * @brief Return a hex dump of @p size bytes prefixed with the byte count, or
 *        "(null)" if @p value is null.
 */
MUTINY_EXPORT String
string_from_binary_with_size_or_null(const unsigned char* value, size_t size);

/** @brief Return the ordinal string for @p number (e.g. "1st", "2nd"). */
MUTINY_EXPORT String string_from_ordinal_number(unsigned int number);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(int value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(unsigned int value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(long value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(unsigned long value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(long long value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String
brackets_formatted_hex_string_from(unsigned long long value);

/** @brief Return @p value as a bracketed hex string, e.g. "(0x0000001f)". */
MUTINY_EXPORT String brackets_formatted_hex_string_from(signed char value);

/**
 * @brief Wrap an already-formatted hex string in brackets
 *        (e.g. "(0x0000001f)").
 */
MUTINY_EXPORT String brackets_formatted_hex_string(const String& hex_string);
} // namespace tiny
} // namespace mu

#undef MUTINY_HAS_ATTRIBUTE
#undef MUTINY_CHECK_FORMAT_TYPE

#endif
