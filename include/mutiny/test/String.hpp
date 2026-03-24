///////////////////////////////////////////////////////////////////////////////
//
// SIMPLESTRING.H
//
// One of the design goals of CppUnitLite is to compilation with very old C++
// compilers.  For that reason, the simple string class that provides
// only the operations needed in CppUnitLite.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_MUTINY_TEST_STRING_HPP
#define INCLUDED_MUTINY_TEST_STRING_HPP

#include <stdarg.h>
#include <stddef.h>

#if MUTINY_USE_STD_CPP_LIB
#include <string>
#endif

#ifndef __has_attribute
#define MUTINY_HAS_ATTRIBUTE(x) 0
#else
#define MUTINY_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif

#if defined(__MINGW32__)
#define MUTINY_CHECK_FORMAT_TYPE __MINGW_PRINTF_FORMAT
#else
#define MUTINY_CHECK_FORMAT_TYPE printf
#endif

namespace mu {
namespace tiny {
namespace test {

#if MUTINY_USE_STD_STRING
#if !MUTINY_USE_STD_CPP_LIB
#error MUTINY_USE_STD_STRING requires MUTINY_USE_STD_CPP_LIB
#endif
using String = std::string;
#else
class String
{
  friend bool operator==(const String& left, const String& right);
  friend bool operator!=(const String& left, const String& right);

public:
  String(const char* value = "");
  String(size_t count, char ch);
  String(const String& other);
  String(String&& other) noexcept;
  ~String();

  String& operator=(const String& other);
  String& operator=(String&& other) noexcept;
  String operator+(const String&) const;
  String& operator+=(const String&);
  String& operator+=(const char*);
  String& operator+=(char ch);

  static const size_t npos = static_cast<size_t>(-1);

  char& operator[](size_t pos) { return data()[pos]; }
  const char& operator[](size_t pos) const { return c_str()[pos]; }

  size_t find(char ch, size_t pos = 0) const;
  size_t find(const char* s, size_t pos = 0) const;

  String substr(size_t begin_pos) const;
  String substr(size_t begin_pos, size_t amount) const;

  const char* c_str() const;
  const char* data() const;
  char* data();
  size_t size() const;
  size_t length() const { return size(); }
  size_t capacity() const { return buffer_size_ ? buffer_size_ - 1 : 0; }
  bool empty() const;
  void clear();
  void reserve(size_t new_capacity);
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

  char* get_empty_string() const;
};
#endif

bool string_contains(const String& str, const String& substr);
bool string_starts_with(const String& str, const String& prefix);
bool string_ends_with(const String& str, const String& suffix);

void string_replace(String& str, char from, char to);
void string_replace(String& str, const char* from, const char* to);

bool iscntrl(char ch);
size_t strlen(const char* str);
const char* strstr(const char* s1, const char* s2);

int atoi(const char* str);
unsigned ato_u(const char* str);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char tolower(char ch);
int memcmp(const void* s1, const void* s2, size_t n);

String string_from(bool value);

String string_from(const void* value);

String string_from(void (*value)());

String string_from(char value);

String string_from(const char* value);

String string_from_or_null(const char* value);

String string_from(int value);

String string_from(unsigned int value);

inline String string_from(signed char value)
{
  return string_from(static_cast<int>(value));
}

inline String string_from(unsigned char value)
{
  return string_from(static_cast<unsigned int>(value));
}

String string_from(long value);

String string_from(unsigned long value);

String string_from(long long value);

String string_from(unsigned long long value);

String hex_string_from(unsigned int value);

String hex_string_from(int value);

String hex_string_from(signed char value);

String hex_string_from(long value);

String hex_string_from(unsigned long value);

String hex_string_from(long long value);

String hex_string_from(unsigned long long value);

String hex_string_from(const void* value);

String hex_string_from(void (*value)());

String string_from(double value, int precision = 6);

String string_from(const String& other);

#if MUTINY_USE_STD_CPP_LIB
String string_from(const std::nullptr_t value);

#if !MUTINY_USE_STD_STRING
String string_from(std::string const& str);
#endif
#endif

inline String string_from(void* value)
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
#include <type_traits>
template<
    typename T,
    typename std::enable_if<std::is_class<T>::value, int>::type = 0>
String string_from(const T&)
{
  static_assert(
      sizeof(T) == 0,
      "No mu::tiny::test::string_from() overload for this type. "
      "Provide: mu::tiny::test::String mu::tiny::test::string_from(const "
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
String string_from_format(
    const char* format,
    ...
) MUTINY_CHECK_FORMAT(MUTINY_CHECK_FORMAT_TYPE, 1, 2);

String v_string_from_format(const char* format, va_list args);

String string_from_binary(const unsigned char* value, size_t size);

String string_from_binary_or_null(const unsigned char* value, size_t size);

String string_from_binary_with_size(const unsigned char* value, size_t size);

String string_from_binary_with_size_or_null(
    const unsigned char* value,
    size_t size
);

String string_from_ordinal_number(unsigned int number);

String brackets_formatted_hex_string_from(int value);

String brackets_formatted_hex_string_from(unsigned int value);

String brackets_formatted_hex_string_from(long value);

String brackets_formatted_hex_string_from(unsigned long value);

String brackets_formatted_hex_string_from(long long value);

String brackets_formatted_hex_string_from(unsigned long long value);

String brackets_formatted_hex_string_from(signed char value);

String brackets_formatted_hex_string(const String& hex_string);
} // namespace test
} // namespace tiny
} // namespace mu

#undef MUTINY_HAS_ATTRIBUTE
#undef MUTINY_CHECK_FORMAT_TYPE

#endif
