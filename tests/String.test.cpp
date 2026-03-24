#include "mutiny/test/String.hpp"

#include "mutiny/test/math.hpp"

#include "mutiny/test.hpp"

#include <limits.h>
#include <stdio.h>

namespace {

int always_true(double)
{
  return true;
}

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
  mu::tiny::test::String expected("aaaa");
  mu::tiny::test::String actual(4, 'a');

  CHECK_EQUAL(expected, actual);
}

TEST(String, CreateRepeatedCharZero)
{
  mu::tiny::test::String expected("");
  mu::tiny::test::String actual(0, 'x'); // NOLINT(bugprone-string-constructor)

  CHECK_EQUAL(expected, actual);
}

TEST(String, Copy)
{
  mu::tiny::test::String s1("hello");
  mu::tiny::test::String s2(s1);

  CHECK_EQUAL(s1, s2);
}

TEST(String, Assignment)
{
  mu::tiny::test::String s1("hello");
  mu::tiny::test::String s2("goodbye");

  s2 = s1;

  CHECK_EQUAL(s1, s2);
}

TEST(String, Equality)
{
  mu::tiny::test::String s1("hello");
  mu::tiny::test::String s2("hello");

  CHECK(s1 == s2);
}

TEST(String, InEquality)
{
  mu::tiny::test::String s1("hello");
  mu::tiny::test::String s2("goodbye");

  CHECK(s1 != s2);
}

TEST(String, c_str)
{
  mu::tiny::test::String s1("hello");

  STRCMP_EQUAL("hello", s1.c_str());
}

TEST(String, Size)
{
  mu::tiny::test::String s1("hello!");

  LONGS_EQUAL(6, s1.size());
}

TEST(String, Addition)
{
  mu::tiny::test::String s1("hello!");
  mu::tiny::test::String s2("goodbye!");
  mu::tiny::test::String s3("hello!goodbye!");
  mu::tiny::test::String s4;
  s4 = s1 + s2;

  CHECK_EQUAL(s3, s4);
}

TEST(String, Concatenation)
{
  mu::tiny::test::String s1("hello!");
  mu::tiny::test::String s2("goodbye!");
  mu::tiny::test::String s3("hello!goodbye!");
  mu::tiny::test::String s4;
  s4 += s1;
  s4 += s2;

  CHECK_EQUAL(s3, s4);

  mu::tiny::test::String s5("hello!goodbye!hello!goodbye!");
  s4 += s4;

  CHECK_EQUAL(s5, s4);
}

TEST(String, stringContains)
{
  mu::tiny::test::String s("hello!");
  mu::tiny::test::String empty("");
  mu::tiny::test::String beginning("hello");
  mu::tiny::test::String end("lo!");
  mu::tiny::test::String mid("l");
  mu::tiny::test::String not_part_of_string("xxxx");

  CHECK(mu::tiny::test::string_contains(s, empty));
  CHECK(mu::tiny::test::string_contains(s, beginning));
  CHECK(mu::tiny::test::string_contains(s, end));
  CHECK(mu::tiny::test::string_contains(s, mid));
  CHECK(!mu::tiny::test::string_contains(s, not_part_of_string));

  CHECK(mu::tiny::test::string_contains(empty, empty));
  CHECK(!mu::tiny::test::string_contains(empty, s));
}

TEST(String, startsWith)
{
  mu::tiny::test::String hi("Hi you!");
  mu::tiny::test::String part("Hi");
  mu::tiny::test::String diff("Hrrm Hi you! ffdsfd");
  CHECK(mu::tiny::test::string_starts_with(hi, part));
  CHECK(!mu::tiny::test::string_starts_with(part, hi));
  CHECK(!mu::tiny::test::string_starts_with(diff, hi));
}

TEST(String, stringEndsWith)
{
  mu::tiny::test::String str("Hello World");
  CHECK(mu::tiny::test::string_ends_with(str, "World"));
  CHECK(!mu::tiny::test::string_ends_with(str, "Worl"));
  CHECK(!mu::tiny::test::string_ends_with(str, "Hello"));
  mu::tiny::test::String str2("ah");
  CHECK(mu::tiny::test::string_ends_with(str2, "ah"));
  CHECK(!mu::tiny::test::string_ends_with(str2, "baah"));
  mu::tiny::test::String str3("");
  CHECK(!mu::tiny::test::string_ends_with(str3, "baah"));

  mu::tiny::test::String str4("ha ha ha ha");
  CHECK(mu::tiny::test::string_ends_with(str4, "ha"));
}

TEST(String, stringReplaceCharWithChar)
{
  mu::tiny::test::String str("abcabcabca");
  mu::tiny::test::string_replace(str, 'a', 'b');
  STRCMP_EQUAL("bbcbbcbbcb", str.c_str());
}

TEST(String, stringReplaceEmptyStringWithEmptyString)
{
  mu::tiny::test::String str;
  mu::tiny::test::string_replace(str, "", "");
  STRCMP_EQUAL("", str.c_str());
}

TEST(String, stringReplaceWholeString)
{
  mu::tiny::test::String str("boo");
  mu::tiny::test::string_replace(str, "boo", "");
  STRCMP_EQUAL("", str.c_str());
}

TEST(String, stringReplaceStringWithString)
{
  mu::tiny::test::String str("boo baa boo baa boo");
  mu::tiny::test::string_replace(str, "boo", "boohoo");
  STRCMP_EQUAL("boohoo baa boohoo baa boohoo", str.c_str());
}

TEST(String, subStringFromEmptyString)
{
  mu::tiny::test::String str("");
  STRCMP_EQUAL("", str.substr(0, 1).c_str());
}

TEST(String, subStringFromSmallString)
{
  mu::tiny::test::String str("H");
  STRCMP_EQUAL("H", str.substr(0, 1).c_str());
}

TEST(String, subStringFromPos0)
{
  mu::tiny::test::String str("Hello World");
  STRCMP_EQUAL("Hello", str.substr(0, 5).c_str());
}

TEST(String, subStringFromPos1)
{
  mu::tiny::test::String str("Hello World");
  STRCMP_EQUAL("ello ", str.substr(1, 5).c_str());
}

TEST(String, subStringFromPos5WithAmountLargerThanString)
{
  mu::tiny::test::String str("Hello World");
  STRCMP_EQUAL("World", str.substr(6, 10).c_str());
}

TEST(String, subStringFromPos6ToEndOfString)
{
  mu::tiny::test::String str("Hello World");
  STRCMP_EQUAL("World", str.substr(6).c_str());
}

TEST(String, findNormal)
{
  mu::tiny::test::String str("Hello World");
  LONGS_EQUAL(0, str.find('H'));
  LONGS_EQUAL(1, str.find('e'));
  LONGS_EQUAL(mu::tiny::test::String::npos, str.find('!'));
}

TEST(String, findCharFromPos)
{
  mu::tiny::test::String str("Hello World");
  LONGS_EQUAL(2, str.find('l', 0));
  LONGS_EQUAL(3, str.find('l', 3));
  LONGS_EQUAL(mu::tiny::test::String::npos, str.find('l', 10));
}

TEST(String, findSubstring)
{
  mu::tiny::test::String str("Hello World");
  LONGS_EQUAL(0, str.find("Hello"));
  LONGS_EQUAL(6, str.find("World"));
  LONGS_EQUAL(mu::tiny::test::String::npos, str.find("xyz"));
}

TEST(String, findSubstringFromPos)
{
  mu::tiny::test::String str("abcabc");
  LONGS_EQUAL(0, str.find("abc", 0));
  LONGS_EQUAL(3, str.find("abc", 1));
  LONGS_EQUAL(mu::tiny::test::String::npos, str.find("abc", 4));
}

TEST(String, findEmptySubstring)
{
  mu::tiny::test::String str("Hello");
  LONGS_EQUAL(0, str.find(""));
  LONGS_EQUAL(3, str.find("", 3));
}

TEST(String, clear)
{
  mu::tiny::test::String str("Hello");
  str.clear();
  STRCMP_EQUAL("", str.c_str());
  LONGS_EQUAL(0, str.size());
}

TEST(String, lessThan)
{
  mu::tiny::test::String a("abc");
  mu::tiny::test::String b("abd");
  mu::tiny::test::String c("abc");
  CHECK(a < b);
  CHECK(!(b < a));
  CHECK(!(a < c));
}

TEST(String, dataConst)
{
  const mu::tiny::test::String str("Hello");
  STRCMP_EQUAL("Hello", str.data());
}

TEST(String, index)
{
  mu::tiny::test::String str("Hello World");
  BYTES_EQUAL('H', str[0]);
}

TEST(String, NULLReportsNullString)
{
  STRCMP_EQUAL(
      "(null)",
      mu::tiny::test::string_from_or_null(static_cast<char*>(nullptr)).c_str()
  );
}

TEST(String, Booleans)
{
  mu::tiny::test::String s1(mu::tiny::test::string_from(true));
  mu::tiny::test::String s2(mu::tiny::test::string_from(false));
  CHECK(s1 == "true");
  CHECK(s2 == "false");
}

TEST(String, Pointers)
{
  mu::tiny::test::String s(
      mu::tiny::test::string_from(reinterpret_cast<void*>(0x1234))
  );
  STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(String, FunctionPointers)
{
  mu::tiny::test::String s(
      mu::tiny::test::string_from(reinterpret_cast<void (*)()>(0x1234))
  );
  STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(String, Characters)
{
  mu::tiny::test::String s(mu::tiny::test::string_from('a'));
  STRCMP_EQUAL("a", s.c_str());
}

TEST(String, NegativeSignedBytes)
{
  STRCMP_EQUAL(
      "-15", mu::tiny::test::string_from(static_cast<signed char>(-15)).c_str()
  );
}

TEST(String, PositiveSignedBytes)
{
  STRCMP_EQUAL("4", mu::tiny::test::string_from(4).c_str());
}

TEST(String, LongInts)
{
  mu::tiny::test::String s(mu::tiny::test::string_from(static_cast<long>(1)));
  CHECK(s == "1");
}

TEST(String, UnsignedLongInts)
{
  mu::tiny::test::String s(
      mu::tiny::test::string_from(static_cast<unsigned long>(1))
  );
  mu::tiny::test::String s2(
      mu::tiny::test::string_from(static_cast<unsigned long>(1))
  );
  CHECK(s == s2);
}

TEST(String, LongLongInts)
{
  mu::tiny::test::String s(
      mu::tiny::test::string_from(static_cast<long long>(1))
  );
  CHECK(s == "1");
}

TEST(String, UnsignedLongLongInts)
{
  mu::tiny::test::String s(
      mu::tiny::test::string_from(static_cast<unsigned long long>(1))
  );
  mu::tiny::test::String s2(
      mu::tiny::test::string_from(static_cast<unsigned long long>(1))
  );
  CHECK(s == s2);
}

TEST(String, Doubles)
{
  mu::tiny::test::String s(mu::tiny::test::string_from(1.2));
  STRCMP_EQUAL("1.2", s.c_str());
}

TEST(String, NaN)
{
  MUTINY_PTR_SET(mu::tiny::test::is_nan, always_true);
  mu::tiny::test::String s(mu::tiny::test::string_from(0.0));
  STRCMP_EQUAL("Nan - Not a number", s.c_str());
}

TEST(String, Inf)
{
  MUTINY_PTR_SET(mu::tiny::test::is_inf, always_true);
  mu::tiny::test::String s(mu::tiny::test::string_from(0.0));
  STRCMP_EQUAL("Inf - Infinity", s.c_str());
}

TEST(String, SmallDoubles)
{
  mu::tiny::test::String s(mu::tiny::test::string_from(1.2e-10));
  STRCMP_CONTAINS("1.2e", s.c_str());
}

TEST(String, Sizes)
{
  size_t size = 10;
  STRCMP_EQUAL(
      "10", mu::tiny::test::string_from(static_cast<int>(size)).c_str()
  );
}

#if MUTINY_USE_STD_CPP_LIB

TEST(String, nullptr_type)
{
  mu::tiny::test::String s(mu::tiny::test::string_from(nullptr));
  STRCMP_EQUAL("(null)", s.c_str());
}

#endif

TEST(String, HexStrings)
{
  STRCMP_EQUAL(
      "f3",
      mu::tiny::test::hex_string_from(static_cast<signed char>(-13)).c_str()
  );

  mu::tiny::test::String h1 = mu::tiny::test::hex_string_from(0xffffL);
  STRCMP_EQUAL("ffff", h1.c_str());

  mu::tiny::test::String h15 = mu::tiny::test::hex_string_from(0xffffLL);
  STRCMP_EQUAL("ffff", h15.c_str());

  mu::tiny::test::String h2 =
      mu::tiny::test::hex_string_from(reinterpret_cast<void*>(0xfffeL));
  STRCMP_EQUAL("fffe", h2.c_str());

  mu::tiny::test::String h3 =
      mu::tiny::test::hex_string_from(reinterpret_cast<void (*)()>(0xfffdL));
  STRCMP_EQUAL("fffd", h3.c_str());
}

TEST(String, StringFromFormat)
{
  mu::tiny::test::String h1 =
      mu::tiny::test::string_from_format("%s %s! %d", "Hello", "World", 2009);
  STRCMP_EQUAL("Hello World! 2009", h1.c_str());
}

TEST(String, StringFromFormatpointer)
{
  // this is not a great test. but %p is odd on mingw and even more odd on
  // Solaris.
  mu::tiny::test::String h1 =
      mu::tiny::test::string_from_format("%p", reinterpret_cast<void*>(1));
  if (h1.size() == 3)
    STRCMP_EQUAL("0x1", h1.c_str());
  else if (h1.size() == 8)
    STRCMP_EQUAL("00000001", h1.c_str());
  else if (h1.size() == 9)
    STRCMP_EQUAL("0000:0001", h1.c_str());
  else if (h1.size() == 16)
    STRCMP_EQUAL("0000000000000001", h1.c_str());
  else if (h1.size() == 1)
    STRCMP_EQUAL("1", h1.c_str());
  else
    FAIL("Off %p behavior");
}

TEST(String, StringFromFormatLarge)
{
  const char* s =
      "ThisIsAPrettyLargeStringAndIfWeAddThisManyTimesToABufferItWillbeFull";
  mu::tiny::test::String h1 = mu::tiny::test::string_from_format(
      "%s%s%s%s%s%s%s%s%s%s", s, s, s, s, s, s, s, s, s, s
  );
  LONGS_EQUAL(
      10 * static_cast<long>(mu::tiny::test::String(s).size()),
      static_cast<long>(h1.size())
  );
}

TEST(String, StringFromConstString)
{
  STRCMP_EQUAL(
      "bla", mu::tiny::test::string_from(mu::tiny::test::String("bla")).c_str()
  );
}

TEST(String, PlatformSpecificSprintf_fits)
{
  char buf[10];

  int count = wrapped_up_vsn_printf(buf, sizeof(buf), "%s", "12345");
  STRCMP_EQUAL("12345", buf);
  LONGS_EQUAL(5, count);
}

TEST(String, PlatformSpecificSprintf_doesNotFit)
{
  char buf[10];

  int count = wrapped_up_vsn_printf(buf, sizeof(buf), "%s", "12345678901");
  STRCMP_EQUAL("123456789", buf);
  LONGS_EQUAL(11, count);
}

TEST(String, NullParameters2)
{
  auto* arr = new mu::tiny::test::String[100];
  delete[] arr;
}

#if defined(__LP64__) || defined(_LP64) ||                                     \
    (defined(__WORDSIZE) && (__WORDSIZE == 64)) || defined(__x86_64) ||        \
    defined(_WIN64)

TEST(String, 64BitAddressPrintsCorrectly)
{
  char* p = reinterpret_cast<char*>(0x0012345678901234LL);
  mu::tiny::test::String expected("0x12345678901234");
  mu::tiny::test::String actual =
      mu::tiny::test::string_from(static_cast<void*>(p));
  STRCMP_EQUAL(expected.c_str(), actual.c_str());
}

#if !defined(_WIN64)

TEST(String, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
  long value = -1;

  STRCMP_EQUAL(
      "(0xffffffffffffffff)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

#else

TEST(String, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
  long value = -1;

  STRCMP_EQUAL(
      "(0xffffffff)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

#endif
#else
/*
 * This test case cannot pass on 32 bit systems.
 */
IGNORE_TEST(String, 64BitAddressPrintsCorrectly) {}

TEST(String, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
  long value = -1;

  STRCMP_EQUAL(
      "(0xffffffff)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}
#endif

TEST(String, BuildStringFromUnsignedLongInteger)
{
  unsigned long int i = 0xffffffff;

  mu::tiny::test::String result = mu::tiny::test::string_from(i);
  const char* expected_string = "4294967295";
  CHECK_EQUAL(expected_string, result);
}

TEST(String, BuildStringFromUnsignedInteger)
{
  unsigned int i = 0xff;

  mu::tiny::test::String result = mu::tiny::test::string_from(i);
  const char* expected_string = "255";
  CHECK_EQUAL(expected_string, result);
}

#if MUTINY_USE_STD_CPP_LIB

TEST(String, fromStdString)
{
  std::string s("hello");
  mu::tiny::test::String s1(mu::tiny::test::string_from(s));

  STRCMP_EQUAL("hello", s1.c_str());
}

TEST( String, CHECK_EQUAL_unsigned_long)
{
  unsigned long i = 0xffffffffUL;
  CHECK_EQUAL(i, i);
}

TEST( String, unsigned_long)
{
  unsigned long i = 0xffffffffUL;

  mu::tiny::test::String result = mu::tiny::test::string_from(i);
  const char* expected_string = "4294967295";
  CHECK_EQUAL(expected_string, result);
}

#endif

TEST(String, StrCmp)
{
  char empty[] = "";
  char blabla[] = "blabla";
  char bla[] = "bla";
  CHECK(mu::tiny::test::strcmp(empty, empty) == 0);
  CHECK(mu::tiny::test::strcmp(bla, blabla) < 0);
  CHECK(mu::tiny::test::strcmp(blabla, bla) > 0);
  CHECK(mu::tiny::test::strcmp(bla, empty) > 0);
  CHECK(mu::tiny::test::strcmp(empty, bla) < 0);
  CHECK(mu::tiny::test::strcmp(bla, bla) == 0);
}

TEST(String, StrNCmp_equal)
{
  int result = mu::tiny::test::strncmp("teststring", "tests", 5);
  LONGS_EQUAL(0, result);
}

TEST(String, StrNCmp_should_always_return_0_when_n_is_0)
{
  int result = mu::tiny::test::strncmp("a", "b", 0);
  LONGS_EQUAL(0, result);
}

TEST(String, StrNCmp_s1_smaller)
{
  int result = mu::tiny::test::strncmp("testing", "tests", 7);
  CHECK(result < 0);
}

TEST(String, StrNCmp_s1_larger)
{
  int result = mu::tiny::test::strncmp("teststring", "tester", 7);
  CHECK(result > 0);
}

TEST(String, StrNCmp_n_too_large)
{
  int result = mu::tiny::test::strncmp("teststring", "teststring", 20);
  LONGS_EQUAL(0, result);
}

TEST(String, StrNCmp_s1_empty)
{
  int result = mu::tiny::test::strncmp("", "foo", 2);
  CHECK(result < 0);
}

TEST(String, StrNCmp_s2_empty)
{
  int result = mu::tiny::test::strncmp("foo", "", 2);
  CHECK(result > 0);
}

TEST(String, StrNCmp_s1_and_s2_empty)
{
  int result = mu::tiny::test::strncmp("", "", 2);
  LONGS_EQUAL(0, result);
}

TEST(String, strtol)
{
  char max_short_str[] = "32767";
  char min_short_str[] = "-32768";

  CHECK(12345 == mu::tiny::test::strtol("012345"));
  CHECK(6789 == mu::tiny::test::strtol("6789"));
  CHECK(12345 == mu::tiny::test::strtol("12345/"));
  CHECK(12345 == mu::tiny::test::strtol("12345:"));
  CHECK(-12345 == mu::tiny::test::strtol("-12345"));
  CHECK(123 == mu::tiny::test::strtol("\t \r\n123"));
  CHECK(123 == mu::tiny::test::strtol("123-foo"));
  CHECK(0 == mu::tiny::test::strtol("-foo"));
  CHECK(-32768 == mu::tiny::test::strtol(min_short_str));
  CHECK(32767 == mu::tiny::test::strtol(max_short_str));
}

TEST(String, strtoul)
{
  char max_short_str[] = "65535";
  CHECK(12345 == mu::tiny::test::strtoul("012345"));
  CHECK(6789 == mu::tiny::test::strtoul("6789"));
  CHECK(12345 == mu::tiny::test::strtoul("12345/"));
  CHECK(12345 == mu::tiny::test::strtoul("12345:"));
  CHECK(1 == mu::tiny::test::strtoul("+1"));
  CHECK(123 == mu::tiny::test::strtoul("\t \r\n123"));
  CHECK(123 == mu::tiny::test::strtoul("123-foo"));
  CHECK(65535 == mu::tiny::test::strtoul(max_short_str));
  CHECK(0 == mu::tiny::test::strtoul("foo"));
  CHECK(0 == mu::tiny::test::strtoul("-foo"));
  CHECK(0 == mu::tiny::test::strtoul("0"));
}

TEST(String, Binary)
{
  const unsigned char value[] = { 0x00, 0x01, 0x2A, 0xFF };
  const char expected_string[] = "00 01 2A FF";

  STRCMP_EQUAL(
      expected_string,
      mu::tiny::test::string_from_binary(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL(
      expected_string,
      mu::tiny::test::string_from_binary_or_null(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL("", mu::tiny::test::string_from_binary(value, 0).c_str());
  STRCMP_EQUAL(
      "(null)", mu::tiny::test::string_from_binary_or_null(nullptr, 0).c_str()
  );
}

TEST(String, BinaryWithSize)
{
  const unsigned char value[] = { 0x12, 0xFE, 0xA1 };
  const char expected_string[] = "Size = 3 | HexContents = 12 FE A1";

  STRCMP_EQUAL(
      expected_string,
      mu::tiny::test::string_from_binary_with_size(value, sizeof(value)).c_str()
  );
  STRCMP_EQUAL(
      expected_string,
      mu::tiny::test::string_from_binary_with_size_or_null(value, sizeof(value))
          .c_str()
  );
  STRCMP_EQUAL(
      "Size = 0 | HexContents = ",
      mu::tiny::test::string_from_binary_with_size(value, 0).c_str()
  );
  STRCMP_EQUAL(
      "(null)",
      mu::tiny::test::string_from_binary_with_size_or_null(nullptr, 0).c_str()
  );
}

TEST(String, BinaryWithSizeLargerThan128)
{
  unsigned char value[129] = {};
  value[127] = 0x00;
  value[128] = 0xff;

  STRCMP_CONTAINS(
      "00 ...",
      mu::tiny::test::string_from_binary_with_size(value, sizeof(value)).c_str()
  );
}

TEST(String, MemCmp)
{
  unsigned char smaller[] = { 0x00, 0x01, 0x2A, 0xFF };
  unsigned char greater[] = { 0x00, 0x01, 0xFF, 0xFF };

  LONGS_EQUAL(0, mu::tiny::test::memcmp(smaller, smaller, sizeof(smaller)));
  CHECK(mu::tiny::test::memcmp(smaller, greater, sizeof(smaller)) < 0);
  CHECK(mu::tiny::test::memcmp(greater, smaller, sizeof(smaller)) > 0);
  LONGS_EQUAL(0, mu::tiny::test::memcmp(nullptr, nullptr, 0));
}

TEST(String, MemCmpFirstLastNotMatching)
{
  unsigned char base[] = { 0x00, 0x01, 0x2A, 0xFF };
  unsigned char first_not_matching[] = { 0x01, 0x01, 0x2A, 0xFF };
  unsigned char last_not_matching[] = { 0x00, 0x01, 0x2A, 0x00 };
  CHECK(0 != mu::tiny::test::memcmp(base, first_not_matching, sizeof(base)));
  CHECK(0 != mu::tiny::test::memcmp(base, last_not_matching, sizeof(base)));
}

TEST(String, StringFromOrdinalNumberOnes)
{
  STRCMP_EQUAL("2nd", mu::tiny::test::string_from_ordinal_number(2).c_str());
  STRCMP_EQUAL("3rd", mu::tiny::test::string_from_ordinal_number(3).c_str());
  STRCMP_EQUAL("4th", mu::tiny::test::string_from_ordinal_number(4).c_str());
  STRCMP_EQUAL("5th", mu::tiny::test::string_from_ordinal_number(5).c_str());
  STRCMP_EQUAL("6th", mu::tiny::test::string_from_ordinal_number(6).c_str());
  STRCMP_EQUAL("7th", mu::tiny::test::string_from_ordinal_number(7).c_str());
}

TEST(String, StringFromOrdinalNumberTens)
{
  STRCMP_EQUAL("10th", mu::tiny::test::string_from_ordinal_number(10).c_str());
  STRCMP_EQUAL("11th", mu::tiny::test::string_from_ordinal_number(11).c_str());
  STRCMP_EQUAL("12th", mu::tiny::test::string_from_ordinal_number(12).c_str());
  STRCMP_EQUAL("13th", mu::tiny::test::string_from_ordinal_number(13).c_str());
  STRCMP_EQUAL("14th", mu::tiny::test::string_from_ordinal_number(14).c_str());
  STRCMP_EQUAL("18th", mu::tiny::test::string_from_ordinal_number(18).c_str());
}

TEST(String, StringFromOrdinalNumberOthers)
{
  STRCMP_EQUAL("21st", mu::tiny::test::string_from_ordinal_number(21).c_str());
  STRCMP_EQUAL("22nd", mu::tiny::test::string_from_ordinal_number(22).c_str());
  STRCMP_EQUAL("23rd", mu::tiny::test::string_from_ordinal_number(23).c_str());
  STRCMP_EQUAL("24th", mu::tiny::test::string_from_ordinal_number(24).c_str());
  STRCMP_EQUAL("32nd", mu::tiny::test::string_from_ordinal_number(32).c_str());
  STRCMP_EQUAL(
      "100th", mu::tiny::test::string_from_ordinal_number(100).c_str()
  );
  STRCMP_EQUAL(
      "101st", mu::tiny::test::string_from_ordinal_number(101).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForSignedChar)
{
  signed char value = 'c';

  STRCMP_EQUAL(
      "(0x63)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForUnsignedInt)
{
  unsigned int value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, BracketsFormattedHexStringFromForUnsignedLong)
{
  unsigned long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

#if (INT_MAX == 0x7fff)
TEST(String, BracketsFormattedHexStringFromForInt)
{
  int value = -1;

  STRCMP_EQUAL(
      "(0xffff)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}
#else
TEST(String, BracketsFormattedHexStringFromForInt)
{
  int value = -1;
  STRCMP_EQUAL(
      "(0xffffffff)",
      mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}
#endif

TEST(String, BracketsFormattedHexStringFromForLong)
{
  long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}
TEST(String, BracketsFormattedHexStringFromForLongLong)
{
  long long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}
TEST(String, BracketsFormattedHexStringFromForULongLong)
{
  unsigned long long value = 1;

  STRCMP_EQUAL(
      "(0x1)", mu::tiny::test::brackets_formatted_hex_string_from(value).c_str()
  );
}

TEST(String, StartsWithEmptyPrefixIsAlwaysTrue)
{
  CHECK(mu::tiny::test::string_starts_with("anything", ""));
}

TEST(String, StartsWithOnEmptyStringIsAlwaysFalse)
{
  CHECK_FALSE(mu::tiny::test::string_starts_with("", "abc"));
}

TEST(String, EndsWithEmptySuffixIsAlwaysTrue)
{
  CHECK(mu::tiny::test::string_ends_with("anything", ""));
}

TEST(String, HexStringFromPositiveSignedChar)
{
  signed char value = 13;
  STRCMP_EQUAL("d", mu::tiny::test::hex_string_from(value).c_str());
}
