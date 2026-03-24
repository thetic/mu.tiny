#include "mutiny/test/String.hpp"

#include "mutiny/test/math.hpp"

#if MUTINY_USE_STD_CPP_LIB
#include <string>
#endif

#if MUTINY_USE_STD_STRING
#include <cctype>
#include <cstdlib>
#include <cstring>
#endif

#include <limits.h>
#include <stdio.h>

namespace mu {
namespace tiny {
namespace test {

namespace {
#if !MUTINY_USE_STD_STRING
char* str_n_cpy(char* s1, const char* s2, size_t n)
{
  char* result = s1;

  if ((nullptr == s1) || (0 == n))
    return result;

  *s1 = *s2;
  while ((--n != 0) && *s1) {
    *++s1 = *++s2;
  }
  return result;
}

char* copy_to_new_buffer(const char* buffer_to_copy, size_t buffer_size)
{
  char* new_buffer = new char[buffer_size];
  str_n_cpy(new_buffer, buffer_to_copy, buffer_size);
  if (buffer_size > 0)
    new_buffer[buffer_size - 1] = '\0';
  return new_buffer;
}
#endif

bool is_digit(char ch)
{
  return '0' <= ch && '9' >= ch;
}

bool is_space(char ch)
{
  return (ch == ' ') || (0x08 < ch && 0x0E > ch);
}

#if !MUTINY_USE_STD_STRING
bool is_upper(char ch)
{
  return 'A' <= ch && 'Z' >= ch;
}
#endif
} // namespace

#if !MUTINY_USE_STD_STRING
char* String::get_empty_string() const
{
  char* buf = new char[1];
  buf[0] = '\0';
  return buf;
}

void String::deallocate_internal_buffer()
{
  if (buffer_) {
    delete[] buffer_;
    buffer_ = nullptr;
    buffer_size_ = 0;
    size_ = 0;
  }
}

void String::set_internal_buffer_as_empty_string()
{
  deallocate_internal_buffer();

  buffer_size_ = 1;
  buffer_ = get_empty_string();
}

void String::copy_buffer_to_new_internal_buffer(
    const char* other_buffer,
    size_t buffer_size
)
{
  deallocate_internal_buffer();

  buffer_size_ = buffer_size;
  size_ = buffer_size_ - 1;
  buffer_ = copy_to_new_buffer(other_buffer, buffer_size_);
}

void String::reserve(size_t new_capacity)
{
  size_t needed = new_capacity + 1;
  if (needed <= buffer_size_)
    return;

  char* new_buffer = new char[needed];
  str_n_cpy(new_buffer, buffer_, buffer_size_);
  new_buffer[needed - 1] = '\0';
  delete[] buffer_;
  buffer_ = new_buffer;
  buffer_size_ = needed;
  // size_ is unchanged: the string content was preserved
}

void String::clear()
{
  set_internal_buffer_as_empty_string();
}

void String::copy_buffer_to_new_internal_buffer(const String& other_buffer)
{
  copy_buffer_to_new_internal_buffer(
      other_buffer.buffer_, other_buffer.size() + 1
  );
}

void String::copy_buffer_to_new_internal_buffer(const char* other_buffer)
{
  copy_buffer_to_new_internal_buffer(other_buffer, strlen(other_buffer) + 1);
}

String::String(const char* other_buffer)
  : buffer_(nullptr)
  , buffer_size_(0)
  , size_(0)
{
  if (other_buffer == nullptr)
    set_internal_buffer_as_empty_string();
  else
    copy_buffer_to_new_internal_buffer(other_buffer);
}

String::String(size_t count, char ch)
  : buffer_(nullptr)
  , buffer_size_(0)
  , size_(0)
{
  buffer_size_ = count + 1;
  size_ = count;
  buffer_ = new char[buffer_size_];
  for (size_t i = 0; i < count; i++)
    buffer_[i] = ch;
  buffer_[count] = '\0';
}

String::String(const String& other)
  : buffer_(nullptr)
  , buffer_size_(0)
  , size_(0)
{
  copy_buffer_to_new_internal_buffer(other.c_str());
}

String::String(String&& other) noexcept
  : buffer_(other.buffer_)
  , buffer_size_(other.buffer_size_)
  , size_(other.size_)
{
  other.buffer_ = nullptr;
  other.buffer_size_ = 0;
  other.size_ = 0;
}

String& String::operator=(String&& other) noexcept
{
  if (this != &other) {
    deallocate_internal_buffer();
    buffer_ = other.buffer_;
    buffer_size_ = other.buffer_size_;
    size_ = other.size_;
    other.buffer_ = nullptr;
    other.buffer_size_ = 0;
    other.size_ = 0;
  }
  return *this;
}

String& String::operator=(const String& other)
{
  if (this != &other)
    copy_buffer_to_new_internal_buffer(other);
  return *this;
}

const char* String::c_str() const
{
  return buffer_;
}

const char* String::data() const
{
  return buffer_;
}

char* String::data()
{
  return buffer_;
}

size_t String::size() const
{
  return size_;
}

bool String::empty() const
{
  return size_ == 0;
}

String::~String()
{
  deallocate_internal_buffer();
}

bool operator==(const String& left, const String& right)
{
  return 0 == strcmp(left.c_str(), right.c_str());
}

bool operator!=(const String& left, const String& right)
{
  return !(left == right);
}

String String::operator+(const String& rhs) const
{
  String t(c_str());
  t += rhs.c_str();
  return t;
}

String& String::operator+=(const String& rhs)
{
  return operator+=(rhs.c_str());
}

String& String::operator+=(const char* rhs)
{
  size_t rhs_len = strlen(rhs);
  size_t new_size = size_ + rhs_len;
  size_t needed = new_size + 1;
  if (needed <= buffer_size_) {
    str_n_cpy(buffer_ + size_, rhs, rhs_len + 1);
  } else {
    size_t new_cap = buffer_size_ * 2;
    if (new_cap < needed)
      new_cap = needed;
    char* nb = new char[new_cap];
    str_n_cpy(nb, buffer_, size_ + 1);
    str_n_cpy(nb + size_, rhs, rhs_len + 1);
    delete[] buffer_;
    buffer_ = nb;
    buffer_size_ = new_cap;
  }
  size_ = new_size;
  return *this;
}

String& String::operator+=(char ch)
{
  size_t needed = size_ + 2;
  if (needed <= buffer_size_) {
    buffer_[size_] = ch;
    buffer_[size_ + 1] = '\0';
    size_ += 1;
    return *this;
  }
  char tmp[2] = { ch, '\0' };
  return operator+=(tmp);
}

void String::resize(size_t new_size)
{
  reserve(new_size);
  buffer_[new_size] = '\0';
  size_ = new_size;
}

String String::substr(size_t begin_pos, size_t amount) const
{
  if (begin_pos > size() - 1)
    return "";

  String new_string = c_str() + begin_pos;

  if (new_string.size_ > amount) {
    new_string.buffer_[amount] = '\0';
    new_string.size_ = amount;
  }

  return new_string;
}

String String::substr(size_t begin_pos) const
{
  return substr(begin_pos, npos);
}

size_t String::find(char ch, size_t pos) const
{
  size_t len = size();
  for (size_t i = pos; i < len; i++)
    if (c_str()[i] == ch)
      return i;
  return npos;
}

size_t String::find(const char* s, size_t pos) const
{
  if (pos > size())
    return npos;
  const char* found = strstr(c_str() + pos, s);
  if (found == nullptr)
    return npos;
  return static_cast<size_t>(found - c_str());
}

bool operator<(const String& left, const String& right)
{
  return strcmp(left.c_str(), right.c_str()) < 0;
}
#endif

bool string_contains(const String& str, const String& substr)
{
  return strstr(str.c_str(), substr.c_str()) != nullptr;
}

bool string_starts_with(const String& str, const String& prefix)
{
  if (prefix.size() == 0)
    return true;
  else if (str.size() == 0)
    return false;
  else
    return strstr(str.c_str(), prefix.c_str()) == str.c_str();
}

bool string_ends_with(const String& str, const String& suffix)
{
  size_t len = str.size();
  size_t other_len = suffix.size();

  if (other_len == 0)
    return true;
  if (len == 0)
    return false;
  if (len < other_len)
    return false;

  return strcmp(str.c_str() + len - other_len, suffix.c_str()) == 0;
}

void string_replace(String& str, char from, char to)
{
  size_t s = str.size();
  for (size_t i = 0; i < s; i++) {
    if (str[i] == from)
      str[i] = to;
  }
}

void string_replace(String& str, const char* from, const char* to)
{
  size_t fromlen = strlen(from);
  if (fromlen == 0)
    return;

  String result;
  size_t pos = 0;
  size_t found = str.find(from, pos);
  while (found != String::npos) {
    result += str.substr(pos, found - pos);
    result += to;
    pos = found + fromlen;
    found = str.find(from, pos);
  }
  if (pos == 0)
    return;
  result += str.substr(pos);
  str = result;
}

// does not support + or - prefixes
unsigned ato_u(const char* str)
{
  while (is_space(*str))
    str++;

  unsigned result = 0;
  for (; is_digit(*str) && *str >= '0'; str++) {
    result *= 10;
    result += static_cast<unsigned>(*str - '0');
  }
  return result;
}

int atoi(const char* str)
{
#if MUTINY_USE_STD_STRING
  return std::atoi(str);
#else
  while (is_space(*str))
    str++;

  char first_char = *str;
  if (first_char == '-' || first_char == '+')
    str++;

  int result = 0;
  for (; is_digit(*str); str++) {
    result *= 10;
    result += *str - '0';
  }
  return (first_char == '-') ? -result : result;
#endif
}

int strcmp(const char* s1, const char* s2)
{
#if MUTINY_USE_STD_STRING
  return std::strcmp(s1, s2);
#else
  while (*s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return *reinterpret_cast<const unsigned char*>(s1) -
         *reinterpret_cast<const unsigned char*>(s2);
#endif
}

size_t strlen(const char* str)
{
#if MUTINY_USE_STD_STRING
  return std::strlen(str);
#else
  auto n = static_cast<size_t>(-1);
  do
    n++;
  while (*str++);
  return n;
#endif
}

int strncmp(const char* s1, const char* s2, size_t n)
{
#if MUTINY_USE_STD_STRING
  return std::strncmp(s1, s2, n);
#else
  while (n && *s1 && *s1 == *s2) {
    --n;
    ++s1;
    ++s2;
  }
  return n ? *reinterpret_cast<const unsigned char*>(s1) -
                 *reinterpret_cast<const unsigned char*>(s2)
           : 0;
#endif
}

const char* strstr(const char* s1, const char* s2)
{
#if MUTINY_USE_STD_STRING
  return std::strstr(s1, s2);
#else
  if (!*s2)
    return s1;
  size_t s2_len = strlen(s2);
  for (; *s1; s1++)
    if (strncmp(s1, s2, s2_len) == 0)
      return s1;
  return nullptr;
#endif
}

char tolower(char ch)
{
#if MUTINY_USE_STD_STRING
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
#else
  return is_upper(ch) ? static_cast<char>(static_cast<int>(ch) + ('a' - 'A'))
                      : ch;
#endif
}

int memcmp(const void* s1, const void* s2, size_t n)
{
#if MUTINY_USE_STD_STRING
  return std::memcmp(s1, s2, n);
#else
  auto* p1 = static_cast<const unsigned char*>(s1);
  auto* p2 = static_cast<const unsigned char*>(s2);

  while (n--)
    if (*p1 != *p2) {
      return *p1 - *p2;
    } else {
      ++p1;
      ++p2;
    }
  return 0;
#endif
}

void pad_strings_to_same_length(String& str1, String& str2, char pad_character)
{
  if (str1.size() > str2.size()) {
    pad_strings_to_same_length(str2, str1, pad_character);
    return;
  }

  str1 = String(str2.size() - str1.size(), pad_character) + str1;
}

bool iscntrl(char ch)
{
#if MUTINY_USE_STD_STRING
  return std::iscntrl(static_cast<unsigned char>(ch)) != 0;
#else
  return ch < ' ' || ch == char(0x7F);
#endif
}

bool is_control_with_short_escape_sequence(char ch)
{
  return '\a' <= ch && '\r' >= ch;
}

String string_from(bool value)
{
  return String(value ? "true" : "false");
}

String string_from(const char* value)
{
  return String(value);
}

String string_from_or_null(const char* expected)
{
  return (expected) ? string_from(expected) : string_from("(null)");
}

String string_from(int value)
{
  return string_from_format("%d", value);
}

String string_from(long value)
{
  return string_from_format("%ld", value);
}

String string_from(const void* value)
{
  return String("0x") + hex_string_from(value);
}

String string_from(void (*value)())
{
  return String("0x") + hex_string_from(value);
}

String hex_string_from(long value)
{
  return hex_string_from(static_cast<unsigned long>(value));
}

String hex_string_from(int value)
{
  return hex_string_from(static_cast<unsigned int>(value));
}

String hex_string_from(signed char value)
{
  String result = string_from_format("%x", value);
  if (value < 0) {
    size_t size = result.size();
    result = result.substr(size - (CHAR_BIT / 4));
  }
  return result;
}

String hex_string_from(unsigned long value)
{
  return string_from_format("%lx", value);
}

String hex_string_from(unsigned int value)
{
  return string_from_format("%x", value);
}

String brackets_formatted_hex_string_from(int value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string_from(unsigned int value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string_from(long value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string_from(unsigned long value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string_from(signed char value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string(const String& hex_string)
{
  return String("(0x") + hex_string + ")";
}

#if MUTINY_USE_STD_CPP_LIB
String string_from(const std::nullptr_t value)
{
  (void)value;
  return "(null)";
}

#if !MUTINY_USE_STD_STRING
String string_from(std::string const& str)
{
  return String(str.c_str());
}
#endif
#endif

String string_from(long long value)
{
  return string_from_format("%lld", value);
}

String string_from(unsigned long long value)
{
  return string_from_format("%llu", value);
}

String hex_string_from(long long value)
{
  return hex_string_from(static_cast<unsigned long long>(value));
}

String hex_string_from(unsigned long long value)
{
  return string_from_format("%llx", value);
}

String hex_string_from(const void* value)
{
  return hex_string_from(reinterpret_cast<unsigned long long>(value));
}

String hex_string_from(void (*value)())
{
  return hex_string_from(reinterpret_cast<unsigned long long>(value));
}

String brackets_formatted_hex_string_from(long long value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String brackets_formatted_hex_string_from(unsigned long long value)
{
  return brackets_formatted_hex_string(hex_string_from(value));
}

String string_from(double value, int precision)
{
  if (mu::tiny::test::is_nan(value))
    return "Nan - Not a number";
  else if (mu::tiny::test::is_inf(value))
    return "Inf - Infinity";
  else
    return string_from_format("%.*g", precision, value);
}

String string_from(char value)
{
  return String(1, value);
}

String string_from(const String& value)
{
  return String(value);
}

String string_from_format(const char* format, ...)
{
  String result_string;
  va_list arguments;
  va_start(arguments, format);

  result_string = v_string_from_format(format, arguments);
  va_end(arguments);
  return result_string;
}

String string_from(unsigned int i)
{
  return string_from_format("%u", i);
}

String string_from(unsigned long i)
{
  return string_from_format("%lu", i);
}

String v_string_from_format(const char* format, va_list args)
{
  va_list args_copy;
  va_copy(args_copy, args);
  constexpr size_t size_ofdefault_buffer = 100;
  char default_buffer[size_ofdefault_buffer];
  String result_string;

  auto size = static_cast<size_t>(
      vsnprintf(default_buffer, size_ofdefault_buffer, format, args)
  );
  if (size < size_ofdefault_buffer) {
    result_string = String(default_buffer);
  } else {
#if !MUTINY_USE_STD_STRING
    result_string.resize(size);
    vsnprintf(result_string.data(), size + 1, format, args_copy);
#else
    size_t new_buffer_size = size + 1;
    char* new_buffer = new char[new_buffer_size];
    vsnprintf(new_buffer, new_buffer_size, format, args_copy);
    result_string = String(new_buffer);
    delete[] new_buffer;
#endif
  }
  va_end(args_copy);
  return result_string;
}

String string_from_binary(const unsigned char* value, size_t size)
{
  static const char hex_digits[] = "0123456789ABCDEF";
  String result;
  if (size == 0)
    return result;
  result.reserve(size * 3 - 1);
  for (size_t i = 0; i < size; i++) {
    if (i > 0)
      result += ' ';
    result += hex_digits[value[i] >> 4];
    result += hex_digits[value[i] & 0xF];
  }
  return result;
}

String string_from_binary_or_null(const unsigned char* value, size_t size)
{
  return (value) ? string_from_binary(value, size) : string_from("(null)");
}

String string_from_binary_with_size(const unsigned char* value, size_t size)
{
  String result = string_from_format(
      "Size = %u | HexContents = ", static_cast<unsigned>(size)
  );
  size_t displayed_size = ((size > 128) ? 128 : size);
  result += string_from_binary_or_null(value, displayed_size);
  if (size > displayed_size) {
    result += " ...";
  }
  return result;
}

String string_from_binary_with_size_or_null(
    const unsigned char* value,
    size_t size
)
{
  return (value) ? string_from_binary_with_size(value, size)
                 : string_from("(null)");
}

String string_from_ordinal_number(unsigned int number)
{
  const char* suffix = "th";

  if ((number < 11) || (number > 13)) {
    unsigned int const ones_digit = number % 10;
    if (3 == ones_digit) {
      suffix = "rd";
    } else if (2 == ones_digit) {
      suffix = "nd";
    } else if (1 == ones_digit) {
      suffix = "st";
    }
  }

  return string_from_format("%u%s", number, suffix);
}

} // namespace test
} // namespace tiny
} // namespace mu
