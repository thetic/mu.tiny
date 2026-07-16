#include "mu/tiny/String.hpp"

#include "mu/tiny/test/Shell.hpp"

#include "mu/tiny/test.hpp"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

namespace {

int wrapped_up_vsn_printf(char* buf, size_t n, const char* format, ...)
{
  va_list arguments;
  va_start(arguments, format);

  int result = vsnprintf(buf, n, format, arguments);
  va_end(arguments);
  return result;
}

} // namespace

TEST_GROUP(String)
{};

TEST(String, CreateRepeatedChar)
{
  mu::tiny::String expected("aaaa");
  mu::tiny::String actual(4, 'a');

  CHECK_EQUAL(expected, actual);
}

TEST(String, CreateRepeatedCharZero)
{
  mu::tiny::String expected("");
  mu::tiny::String actual(0, 'x'); // NOLINT(bugprone-string-constructor)

  CHECK_EQUAL(expected, actual);
}

TEST(String, Copy)
{
  mu::tiny::String s1("hello");
  mu::tiny::String s2(s1);

  CHECK_EQUAL(s1, s2);
}

TEST(String, Assignment)
{
  mu::tiny::String s1("hello");
  mu::tiny::String s2("goodbye");

  s2 = s1;

  CHECK_EQUAL(s1, s2);
}

TEST(String, Equality)
{
  mu::tiny::String s1("hello");
  mu::tiny::String s2("hello");

  CHECK(s1 == s2);
}

TEST(String, InEquality)
{
  mu::tiny::String s1("hello");
  mu::tiny::String s2("goodbye");

  CHECK(s1 != s2);
}

TEST(String, c_str)
{
  mu::tiny::String s1("hello");

  STRCMP_EQUAL("hello", s1.c_str());
}

TEST(String, Size)
{
  mu::tiny::String s1("hello!");

  CHECK_EQUAL(size_t{ 6 }, s1.size());
}

TEST(String, Addition)
{
  mu::tiny::String s1("hello!");
  mu::tiny::String s2("goodbye!");
  mu::tiny::String s3("hello!goodbye!");
  mu::tiny::String s4;
  s4 = s1 + s2;

  CHECK_EQUAL(s3, s4);
}

TEST(String, Concatenation)
{
  mu::tiny::String s1("hello!");
  mu::tiny::String s2("goodbye!");
  mu::tiny::String s3("hello!goodbye!");
  mu::tiny::String s4;
  s4 += s1;
  s4 += s2;

  CHECK_EQUAL(s3, s4);

  mu::tiny::String s5("hello!goodbye!hello!goodbye!");
  s4 += s4;

  CHECK_EQUAL(s5, s4);
}

TEST(String, stringContains)
{
  mu::tiny::String s("hello!");
  mu::tiny::String empty("");
  mu::tiny::String beginning("hello");
  mu::tiny::String end("lo!");
  mu::tiny::String mid("l");
  mu::tiny::String not_part_of_string("xxxx");

  CHECK(mu::tiny::string_contains(s, empty));
  CHECK(mu::tiny::string_contains(s, beginning));
  CHECK(mu::tiny::string_contains(s, end));
  CHECK(mu::tiny::string_contains(s, mid));
  CHECK(!mu::tiny::string_contains(s, not_part_of_string));

  CHECK(mu::tiny::string_contains(empty, empty));
  CHECK(!mu::tiny::string_contains(empty, s));
}

TEST(String, startsWith)
{
  mu::tiny::String hi("Hi you!");
  mu::tiny::String part("Hi");
  mu::tiny::String diff("Hrrm Hi you! ffdsfd");
  CHECK(mu::tiny::string_starts_with(hi, part));
  CHECK(!mu::tiny::string_starts_with(part, hi));
  CHECK(!mu::tiny::string_starts_with(diff, hi));
}

TEST(String, stringEndsWith)
{
  mu::tiny::String str("Hello World");
  CHECK(mu::tiny::string_ends_with(str, "World"));
  CHECK(!mu::tiny::string_ends_with(str, "Worl"));
  CHECK(!mu::tiny::string_ends_with(str, "Hello"));

  mu::tiny::String str2("ah");
  CHECK(mu::tiny::string_ends_with(str2, "ah"));
  CHECK(!mu::tiny::string_ends_with(str2, "baah"));

  mu::tiny::String str3("");
  CHECK(!mu::tiny::string_ends_with(str3, "baah"));

  mu::tiny::String str4("ha ha ha ha");
  CHECK(mu::tiny::string_ends_with(str4, "ha"));
}

TEST(String, stringReplaceCharWithChar)
{
  mu::tiny::String str("abcabcabca");
  mu::tiny::string_replace(str, 'a', 'b');
  STRCMP_EQUAL("bbcbbcbbcb", str.c_str());
}

TEST(String, stringReplaceEmptyStringWithEmptyString)
{
  mu::tiny::String str;
  mu::tiny::string_replace(str, "", "");
  STRCMP_EQUAL("", str.c_str());
}

TEST(String, stringReplaceWholeString)
{
  mu::tiny::String str("boo");
  mu::tiny::string_replace(str, "boo", "");
  STRCMP_EQUAL("", str.c_str());
}

TEST(String, stringReplaceStringWithString)
{
  mu::tiny::String str("boo baa boo baa boo");
  mu::tiny::string_replace(str, "boo", "boohoo");
  STRCMP_EQUAL("boohoo baa boohoo baa boohoo", str.c_str());
}

TEST(String, subStringFromEmptyString)
{
  mu::tiny::String str("");
  STRCMP_EQUAL("", str.substr(0, 1).c_str());
}

#if !MUTINY_USE_STD_CPP_LIB
TEST(String, subStringFromEmptyStringWithNonZeroPos)
{
  mu::tiny::String str("");
  STRCMP_EQUAL("", str.substr(1, 1).c_str());
}
#endif

TEST(String, subStringFromSmallString)
{
  mu::tiny::String str("H");
  STRCMP_EQUAL("H", str.substr(0, 1).c_str());
}

TEST(String, subStringFromPos0)
{
  mu::tiny::String str("Hello World");
  STRCMP_EQUAL("Hello", str.substr(0, 5).c_str());
}

TEST(String, subStringFromPos1)
{
  mu::tiny::String str("Hello World");
  STRCMP_EQUAL("ello ", str.substr(1, 5).c_str());
}

TEST(String, subStringFromPos5WithAmountLargerThanString)
{
  mu::tiny::String str("Hello World");
  STRCMP_EQUAL("World", str.substr(6, 10).c_str());
}

TEST(String, subStringFromPos6ToEndOfString)
{
  mu::tiny::String str("Hello World");
  STRCMP_EQUAL("World", str.substr(6).c_str());
}

TEST(String, findNormal)
{
  mu::tiny::String str("Hello World");
  CHECK_EQUAL(size_t{ 0 }, str.find('H'));
  CHECK_EQUAL(size_t{ 1 }, str.find('e'));
  CHECK_EQUAL(mu::tiny::String::npos, str.find('!'));
}

TEST(String, findCharFromPos)
{
  mu::tiny::String str("Hello World");
  CHECK_EQUAL(size_t{ 2 }, str.find('l', 0));
  CHECK_EQUAL(size_t{ 3 }, str.find('l', 3));
  CHECK_EQUAL(mu::tiny::String::npos, str.find('l', 10));
}

TEST(String, findSubstring)
{
  mu::tiny::String str("Hello World");
  CHECK_EQUAL(size_t{ 0 }, str.find("Hello"));
  CHECK_EQUAL(size_t{ 6 }, str.find("World"));
  CHECK_EQUAL(mu::tiny::String::npos, str.find("xyz"));
}

TEST(String, findSubstringFromPos)
{
  mu::tiny::String str("abcabc");
  CHECK_EQUAL(size_t{ 0 }, str.find("abc", 0));
  CHECK_EQUAL(size_t{ 3 }, str.find("abc", 1));
  CHECK_EQUAL(mu::tiny::String::npos, str.find("abc", 4));
}

TEST(String, findEmptySubstring)
{
  mu::tiny::String str("Hello");
  CHECK_EQUAL(size_t{ 0 }, str.find(""));
  CHECK_EQUAL(size_t{ 3 }, str.find("", 3));
}

TEST(String, clear)
{
  mu::tiny::String str("Hello");
  str.clear();
  STRCMP_EQUAL("", str.c_str());
  CHECK_EQUAL(size_t{ 0 }, str.size());
}

TEST(String, lessThan)
{
  mu::tiny::String a("abc");
  mu::tiny::String b("abd");
  mu::tiny::String c("abc");
  CHECK(a < b);
  CHECK(!(b < a));
  CHECK(!(a < c));
}

TEST(String, dataConst)
{
  const mu::tiny::String str("Hello");
  STRCMP_EQUAL("Hello", str.data());
}

TEST(String, index)
{
  mu::tiny::String str("Hello World");
  CHECK_EQUAL('H', str[0]);
}

TEST(String, NULLReportsNullString)
{
  STRCMP_EQUAL(
      "(null)",
      mu::tiny::string_from_or_null(static_cast<char*>(nullptr)).c_str()
  );
}

TEST(String, Booleans)
{
  mu::tiny::String s1(mu::tiny::string_from(true));
  mu::tiny::String s2(mu::tiny::string_from(false));
  CHECK(s1 == "true");
  CHECK(s2 == "false");
}

TEST(String, Pointers)
{
  mu::tiny::String s(mu::tiny::string_from(reinterpret_cast<void*>(0x1234)));
  STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(String, FunctionPointers)
{
  mu::tiny::String s(
      mu::tiny::string_from(reinterpret_cast<void (*)()>(0x1234))
  );
  STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(String, Characters)
{
  mu::tiny::String s(mu::tiny::string_from('a'));
  STRCMP_EQUAL("a", s.c_str());
}

TEST(String, NegativeSignedBytes)
{
  STRCMP_EQUAL(
      "-15", mu::tiny::string_from(static_cast<signed char>(-15)).c_str()
  );
}

TEST(String, UnsignedBytes)
{
  STRCMP_EQUAL(
      "200", mu::tiny::string_from(static_cast<unsigned char>(200)).c_str()
  );
}

TEST(String, PositiveSignedBytes)
{
  STRCMP_EQUAL("4", mu::tiny::string_from(4).c_str());
}

TEST(String, LongInts)
{
  mu::tiny::String s(mu::tiny::string_from(1L));
  CHECK(s == "1");
}

TEST(String, UnsignedLongInts)
{
  mu::tiny::String s(mu::tiny::string_from(1UL));
  mu::tiny::String s2(mu::tiny::string_from(1UL));
  CHECK(s == s2);
}

TEST(String, LongLongInts)
{
  mu::tiny::String s(mu::tiny::string_from(1LL));
  CHECK(s == "1");
}

TEST(String, UnsignedLongLongInts)
{
  mu::tiny::String s(mu::tiny::string_from(1ULL));
  mu::tiny::String s2(mu::tiny::string_from(1ULL));
  CHECK(s == s2);
}

TEST(String, Floats)
{
  mu::tiny::String s(mu::tiny::string_from(1.2F));
  STRCMP_EQUAL("1.2", s.c_str());
}

TEST(String, Doubles)
{
  mu::tiny::String s(mu::tiny::string_from(1.2));
  STRCMP_EQUAL("1.2", s.c_str());
}

TEST(String, NaN)
{
  mu::tiny::String s(mu::tiny::string_from(NAN));
  STRCMP_EQUAL("Nan - Not a number", s.c_str());
}

TEST(String, Inf)
{
  mu::tiny::String s(mu::tiny::string_from(INFINITY));
  STRCMP_EQUAL("Inf - Infinity", s.c_str());
}

TEST(String, SmallDoubles)
{
  mu::tiny::String s(mu::tiny::string_from(1.2e-10));
  STRCMP_CONTAINS("1.2e", s.c_str());
}

TEST(String, Sizes)
{
  size_t size = 10;
  STRCMP_EQUAL("10", mu::tiny::string_from(static_cast<int>(size)).c_str());
}

#if MUTINY_USE_STD_CPP_LIB

TEST(String, nullptr_type)
{
  mu::tiny::String s(mu::tiny::string_from(nullptr));
  STRCMP_EQUAL("(null)", s.c_str());
}

#endif

TEST(String, HexStrings)
{
  STRCMP_EQUAL(
      "f3", mu::tiny::hex_string_from(static_cast<signed char>(-13)).c_str()
  );

  mu::tiny::String h1 = mu::tiny::hex_string_from(0xffffL);
  STRCMP_EQUAL("ffff", h1.c_str());

  mu::tiny::String h15 = mu::tiny::hex_string_from(0xffffLL);
  STRCMP_EQUAL("ffff", h15.c_str());

  mu::tiny::String h2 =
      mu::tiny::hex_string_from(reinterpret_cast<void*>(0xfffeL));
  STRCMP_EQUAL("fffe", h2.c_str());

  mu::tiny::String h3 =
      mu::tiny::hex_string_from(reinterpret_cast<void (*)()>(0xfffdL));
  STRCMP_EQUAL("fffd", h3.c_str());
}

TEST(String, StringFromFormat)
{
  mu::tiny::String h1 =
      mu::tiny::string_from_format("%s %s! %d", "Hello", "World", 2009);
  STRCMP_EQUAL("Hello World! 2009", h1.c_str());
}

TEST(String, StringFromFormatpointer)
{
  // this is not a great test. but %p is odd on mingw and even more odd on
  // Solaris.
  mu::tiny::String h1 =
      mu::tiny::string_from_format("%p", reinterpret_cast<void*>(1));
  if (h1.size() == 3) {
    STRCMP_EQUAL("0x1", h1.c_str());
  } else if (h1.size() == 8) {
    STRCMP_EQUAL("00000001", h1.c_str());
  } else if (h1.size() == 9) {
    STRCMP_EQUAL("0000:0001", h1.c_str());
  } else if (h1.size() == 16) {
    STRCMP_EQUAL("0000000000000001", h1.c_str());
  } else if (h1.size() == 1) {
    STRCMP_EQUAL("1", h1.c_str());
  } else {
    FAIL_TEST("Off %p behavior");
  }
}

TEST(String, StringFromFormatLarge)
{
  const char* s =
      "ThisIsAPrettyLargeStringAndIfWeAddThisManyTimesToABufferItWillbeFull";
  mu::tiny::String h1 = mu::tiny::string_from_format(
      "%s%s%s%s%s%s%s%s%s%s", s, s, s, s, s, s, s, s, s, s
  );
  CHECK_EQUAL(
      10 * static_cast<long>(mu::tiny::String(s).size()),
      static_cast<long>(h1.size())
  );
}

TEST(String, StringFromConstString)
{
  STRCMP_EQUAL("bla", mu::tiny::string_from(mu::tiny::String("bla")).c_str());
}

TEST(String, PlatformSpecificSprintf_fits)
{
  char buf[10];

  int count = wrapped_up_vsn_printf(buf, sizeof(buf), "%s", "12345");
  STRCMP_EQUAL("12345", buf);
  CHECK_EQUAL(5, count);
}

TEST(String, PlatformSpecificSprintf_doesNotFit)
{
  char buf[10];

  int count = wrapped_up_vsn_printf(buf, sizeof(buf), "%s", "12345678901");
  STRCMP_EQUAL("123456789", buf);
  CHECK_EQUAL(11, count);
}

TEST(String, NullParameters2)
{
  auto* arr = new mu::tiny::String[100];
  delete[] arr;
}

TEST(String, 64BitAddressPrintsCorrectly)
{
  const bool small_pointer = sizeof(void*) < sizeof(uint64_t);
  if (small_pointer) {
    SKIP_TEST("requires 64-bit pointers");
  }
  char* p = reinterpret_cast<char*>(0x0012345678901234LL);
  mu::tiny::String expected("0x12345678901234");
  mu::tiny::String actual = mu::tiny::string_from(static_cast<void*>(p));
  STRCMP_EQUAL(expected.c_str(), actual.c_str());
}

TEST(String, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
  long value = -1;
  const char* expected = sizeof(long) == sizeof(uint64_t)
                             ? "(0xffffffffffffffff)"
                             : "(0xffffffff)";

  STRCMP_EQUAL(
      expected, mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BuildStringFromUnsignedLongInteger)
{
  unsigned long int i = 0xffffffff;

  mu::tiny::String result = mu::tiny::string_from(i);
  const char* expected_string = "4294967295";
  CHECK_EQUAL(expected_string, result);
}

TEST(String, BuildStringFromUnsignedInteger)
{
  unsigned int i = 0xff;

  mu::tiny::String result = mu::tiny::string_from(i);
  const char* expected_string = "255";
  CHECK_EQUAL(expected_string, result);
}

#if MUTINY_USE_STD_CPP_LIB

TEST(String, fromStdString)
{
  std::string s("hello");
  mu::tiny::String s1(mu::tiny::string_from(s));

  STRCMP_EQUAL("hello", s1.c_str());
}

TEST(String, CHECK_EQUAL_unsigned_long)
{
  unsigned long i = 0xffffffffUL;
  CHECK_EQUAL(i, i);
}

TEST(String, unsigned_long)
{
  unsigned long i = 0xffffffffUL;

  mu::tiny::String result = mu::tiny::string_from(i);
  const char* expected_string = "4294967295";
  CHECK_EQUAL(expected_string, result);
}

#endif

TEST(String, StrCmp)
{
  char empty[] = "";
  char blabla[] = "blabla";
  char bla[] = "bla";
  CHECK(mu::tiny::strcmp(empty, empty) == 0);
  CHECK(mu::tiny::strcmp(bla, blabla) < 0);
  CHECK(mu::tiny::strcmp(blabla, bla) > 0);
  CHECK(mu::tiny::strcmp(bla, empty) > 0);
  CHECK(mu::tiny::strcmp(empty, bla) < 0);
  CHECK(mu::tiny::strcmp(bla, bla) == 0);
}

TEST(String, StrNCmp_equal)
{
  int result = mu::tiny::strncmp("teststring", "tests", 5);
  CHECK_EQUAL(0, result);
}

TEST(String, StrNCmp_should_always_return_0_when_n_is_0)
{
  int result = mu::tiny::strncmp("a", "b", 0);
  CHECK_EQUAL(0, result);
}

TEST(String, StrNCmp_s1_smaller)
{
  int result = mu::tiny::strncmp("testing", "tests", 7);
  CHECK(result < 0);
}

TEST(String, StrNCmp_s1_larger)
{
  int result = mu::tiny::strncmp("teststring", "tester", 7);
  CHECK(result > 0);
}

TEST(String, StrNCmp_n_too_large)
{
  int result = mu::tiny::strncmp("teststring", "teststring", 20);
  CHECK_EQUAL(0, result);
}

TEST(String, StrNCmp_s1_empty)
{
  int result = mu::tiny::strncmp("", "foo", 2);
  CHECK(result < 0);
}

TEST(String, StrNCmp_s2_empty)
{
  int result = mu::tiny::strncmp("foo", "", 2);
  CHECK(result > 0);
}

TEST(String, StrNCmp_s1_and_s2_empty)
{
  int result = mu::tiny::strncmp("", "", 2);
  CHECK_EQUAL(0, result);
}

TEST(String, strtoul)
{
  char max_short_str[] = "65535";
  CHECK(12345 == mu::tiny::strtoul("012345"));
  CHECK(6789 == mu::tiny::strtoul("6789"));
  CHECK(12345 == mu::tiny::strtoul("12345/"));
  CHECK(12345 == mu::tiny::strtoul("12345:"));
  CHECK(1 == mu::tiny::strtoul("+1"));
  CHECK(123 == mu::tiny::strtoul("\t \r\n123"));
  CHECK(123 == mu::tiny::strtoul("123-foo"));
  CHECK(65535 == mu::tiny::strtoul(max_short_str));
  CHECK(0 == mu::tiny::strtoul("foo"));
  CHECK(0 == mu::tiny::strtoul("-foo"));
  CHECK(0 == mu::tiny::strtoul("0"));
}

TEST(String, Binary)
{
  const unsigned char value[] = { 0x00, 0x01, 0x2A, 0xFF };
  const char expected_string[] = "00 01 2A FF";

  STRCMP_EQUAL(
      expected_string,
      mu::tiny::string_from_binary(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL(
      expected_string,
      mu::tiny::string_from_binary_or_null(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL("", mu::tiny::string_from_binary(value, 0).c_str());
  STRCMP_EQUAL(
      "(null)", mu::tiny::string_from_binary_or_null(nullptr, 0).c_str()
  );
}

TEST(String, BinaryWithSize)
{
  const unsigned char value[] = { 0x12, 0xFE, 0xA1 };
  const char expected_string[] = "Size = 3 | HexContents = 12 FE A1";

  STRCMP_EQUAL(
      expected_string,
      mu::tiny::string_from_binary_with_size(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL(
      expected_string,
      mu::tiny::string_from_binary_with_size_or_null(value, sizeof(value))
          .c_str()
  );
  STRCMP_EQUAL(
      "Size = 0 | HexContents = ",
      mu::tiny::string_from_binary_with_size(value, 0).c_str()
  );
  STRCMP_EQUAL(
      "(null)",
      mu::tiny::string_from_binary_with_size_or_null(nullptr, 0).c_str()
  );
}

TEST(String, BinaryWithSizeLargerThan128)
{
  unsigned char value[129] = {};
  value[127] = 0x00;
  value[128] = 0xff;

  STRCMP_CONTAINS(
      "00 ...",
      mu::tiny::string_from_binary_with_size(value, sizeof(value)).c_str()
  );
}

TEST(String, MemCmp)
{
  unsigned char smaller[] = { 0x00, 0x01, 0x2A, 0xFF };
  unsigned char greater[] = { 0x00, 0x01, 0xFF, 0xFF };

  CHECK_EQUAL(0, mu::tiny::memcmp(smaller, smaller, sizeof(smaller)));
  CHECK(mu::tiny::memcmp(smaller, greater, sizeof(smaller)) < 0);
  CHECK(mu::tiny::memcmp(greater, smaller, sizeof(smaller)) > 0);
  CHECK_EQUAL(0, mu::tiny::memcmp(nullptr, nullptr, 0));
}

TEST(String, MemCmpFirstLastNotMatching)
{
  unsigned char base[] = { 0x00, 0x01, 0x2A, 0xFF };
  unsigned char first_not_matching[] = { 0x01, 0x01, 0x2A, 0xFF };
  unsigned char last_not_matching[] = { 0x00, 0x01, 0x2A, 0x00 };
  CHECK(0 != mu::tiny::memcmp(base, first_not_matching, sizeof(base)));
  CHECK(0 != mu::tiny::memcmp(base, last_not_matching, sizeof(base)));
}

TEST(String, StringFromOrdinalNumberOnes)
{
  STRCMP_EQUAL("2nd", mu::tiny::string_from_ordinal_number(2).c_str());
  STRCMP_EQUAL("3rd", mu::tiny::string_from_ordinal_number(3).c_str());
  STRCMP_EQUAL("4th", mu::tiny::string_from_ordinal_number(4).c_str());
  STRCMP_EQUAL("5th", mu::tiny::string_from_ordinal_number(5).c_str());
  STRCMP_EQUAL("6th", mu::tiny::string_from_ordinal_number(6).c_str());
  STRCMP_EQUAL("7th", mu::tiny::string_from_ordinal_number(7).c_str());
}

TEST(String, StringFromOrdinalNumberTens)
{
  STRCMP_EQUAL("10th", mu::tiny::string_from_ordinal_number(10).c_str());
  STRCMP_EQUAL("11th", mu::tiny::string_from_ordinal_number(11).c_str());
  STRCMP_EQUAL("12th", mu::tiny::string_from_ordinal_number(12).c_str());
  STRCMP_EQUAL("13th", mu::tiny::string_from_ordinal_number(13).c_str());
  STRCMP_EQUAL("14th", mu::tiny::string_from_ordinal_number(14).c_str());
  STRCMP_EQUAL("18th", mu::tiny::string_from_ordinal_number(18).c_str());
}

TEST(String, StringFromOrdinalNumberOthers)
{
  STRCMP_EQUAL("21st", mu::tiny::string_from_ordinal_number(21).c_str());
  STRCMP_EQUAL("22nd", mu::tiny::string_from_ordinal_number(22).c_str());
  STRCMP_EQUAL("23rd", mu::tiny::string_from_ordinal_number(23).c_str());
  STRCMP_EQUAL("24th", mu::tiny::string_from_ordinal_number(24).c_str());
  STRCMP_EQUAL("32nd", mu::tiny::string_from_ordinal_number(32).c_str());
  STRCMP_EQUAL("100th", mu::tiny::string_from_ordinal_number(100).c_str());
  STRCMP_EQUAL("101st", mu::tiny::string_from_ordinal_number(101).c_str());
}

TEST(String, BracketsFormattedHexStringFromForSignedChar)
{
  signed char value = 'c';

  STRCMP_EQUAL(
      "(0x63)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForUnsignedInt)
{
  unsigned int value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForUnsignedLong)
{
  unsigned long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForInt)
{
  int value = -1;
  const char* expected =
      sizeof(int) == sizeof(uint16_t) ? "(0xffff)" : "(0xffffffff)";

  STRCMP_EQUAL(
      expected, mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForLong)
{
  long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}
TEST(String, BracketsFormattedHexStringFromForLongLong)
{
  long long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}
TEST(String, BracketsFormattedHexStringFromForULongLong)
{
  unsigned long long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, StartsWithEmptyPrefixIsAlwaysTrue)
{
  CHECK(mu::tiny::string_starts_with("anything", ""));
}

TEST(String, StartsWithOnEmptyStringIsAlwaysFalse)
{
  CHECK(!mu::tiny::string_starts_with("", "abc"));
}

TEST(String, EndsWithEmptySuffixIsAlwaysTrue)
{
  CHECK(mu::tiny::string_ends_with("anything", ""));
}

TEST(String, HexStringFromPositiveSignedChar)
{
  signed char value = 13;
  STRCMP_EQUAL("d", mu::tiny::hex_string_from(value).c_str());
}
