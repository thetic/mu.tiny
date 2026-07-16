#include "mu/tiny/test/Output.hpp"

#include "mu/tiny/test.hpp"

#include <limits.h>

namespace {
const int fail_line_number = 2;
const char* fail_file_name = "fail.cpp";
} // namespace

TEST_GROUP(Failure)
{
  mu::tiny::test::Shell* test;

  void setup() override
  {
    test = new mu::tiny::test::Shell(
        "groupname", "testname", fail_file_name, fail_line_number - 1
    );
  }
  void teardown() override { delete test; }
};
#define FAILURE_EQUAL(a, b)                                                    \
  STRCMP_EQUAL_LOCATION(a, (b).get_message().c_str(), "", __FILE__, __LINE__)

TEST(Failure, CreateFailure)
{
  mu::tiny::test::Failure f1(
      test, fail_file_name, fail_line_number, "the failure message"
  );
  mu::tiny::test::Failure f2(test, "the failure message");
  mu::tiny::test::Failure f3(test, fail_file_name, fail_line_number);
}

TEST(Failure, GetTestFileAndLineFromFailure)
{
  mu::tiny::test::Failure f1(
      test, fail_file_name, fail_line_number, "the failure message"
  );
  STRCMP_EQUAL(fail_file_name, f1.get_test_file_name().c_str());
  CHECK_EQUAL(1, f1.get_test_line_number());
}

TEST(Failure, EqualsFailureWithText)
{
  mu::tiny::test::EqualsFailure f(
      test, fail_file_name, fail_line_number, "expected", "actual", "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <expected>\n\tbut was  <actual>",
      f
  );
}

TEST(Failure, EqualsFailure)
{
  mu::tiny::test::EqualsFailure f(
      test, fail_file_name, fail_line_number, "expected", "actual", ""
  );
  FAILURE_EQUAL("expected <expected>\n\tbut was  <actual>", f);
}

TEST(Failure, EqualsFailureWithNullAsActual)
{
  mu::tiny::test::EqualsFailure f(
      test, fail_file_name, fail_line_number, "expected", nullptr, ""
  );
  FAILURE_EQUAL("expected <expected>\n\tbut was  <(null)>", f);
}

TEST(Failure, EqualsFailureWithNullAsExpected)
{
  mu::tiny::test::EqualsFailure f(
      test, fail_file_name, fail_line_number, nullptr, "actual", ""
  );
  FAILURE_EQUAL("expected <(null)>\n\tbut was  <actual>", f);
}

TEST(Failure, CheckEqualFailureWithText)
{
  mu::tiny::test::CheckEqualFailure f(
      test, fail_file_name, fail_line_number, "expected", "actual", "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <expected>\n"
      "\tbut was  <actual>\n"
      "\tdifference starts at position 0 at: <          actual    >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, CheckEqualFailure)
{
  mu::tiny::test::CheckEqualFailure f(
      test, fail_file_name, fail_line_number, "expected", "actual", ""
  );
  FAILURE_EQUAL(
      "expected <expected>\n"
      "\tbut was  <actual>\n"
      "\tdifference starts at position 0 at: <          actual    >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, CheckEqualFailureIdenticalRepresentations)
{
  mu::tiny::test::CheckEqualFailure f(
      test, fail_file_name, fail_line_number, "1", "1", ""
  );
  CHECK(
      mu::tiny::string_contains(
          f.get_message(), "difference starts at position 1"
      )
  );
}

TEST(Failure, CheckFailure)
{
  mu::tiny::test::CheckFailure f(
      test, fail_file_name, fail_line_number, "CHECK", "chk"
  );
  FAILURE_EQUAL("CHECK(chk) failed", f);
}

TEST(Failure, CheckFailureWithText)
{
  mu::tiny::test::CheckFailure f(
      test, fail_file_name, fail_line_number, "CHECK", "chk", "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\tCHECK(chk) failed",
      f
  );
}

TEST(Failure, FailFailure)
{
  mu::tiny::test::FailFailure f(test, fail_file_name, fail_line_number, "chk");
  FAILURE_EQUAL("chk", f);
}

TEST(Failure, LongsEqualFailureWithText)
{
  mu::tiny::test::IntMaxEqualFailure f(
      test, fail_file_name, fail_line_number, 1, 2, "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <1 (0x1)>\n\tbut was  <2 (0x2)>",
      f
  );
}

TEST(Failure, LongsEqualFailure)
{
  mu::tiny::test::IntMaxEqualFailure f(
      test, fail_file_name, fail_line_number, 1, 2, ""
  );
  FAILURE_EQUAL("expected <1 (0x1)>\n\tbut was  <2 (0x2)>", f);
}

TEST(Failure, LongLongsEqualFailure)
{
  mu::tiny::test::IntMaxEqualFailure f(
      test, fail_file_name, fail_line_number, 1, 2, ""
  );
  FAILURE_EQUAL("expected <1 (0x1)>\n\tbut was  <2 (0x2)>", f);
}

TEST(Failure, LongsEqualFailureActualLongerThanExpected)
{
  mu::tiny::test::IntMaxEqualFailure f(
      test, fail_file_name, fail_line_number, 1, 1000, ""
  );
  FAILURE_EQUAL("expected <   1 (0x1)>\n\tbut was  <1000 (0x3e8)>", f);
}

TEST(Failure, UnsignedLongLongsEqualFailure)
{
  mu::tiny::test::UintMaxEqualFailure f(
      test, fail_file_name, fail_line_number, 1, 2, ""
  );
  FAILURE_EQUAL("expected <1 (0x1)>\n\tbut was  <2 (0x2)>", f);
}

TEST(Failure, StringsEqualFailureWithText)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "abc", "abd", "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <abc>\n"
      "\tbut was  <abd>\n"
      "\tdifference starts at position 2 at: <        abd         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualFailure)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "abc", "abd", ""
  );
  FAILURE_EQUAL(
      "expected <abc>\n"
      "\tbut was  <abd>\n"
      "\tdifference starts at position 2 at: <        abd         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualFailureAtTheEnd)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "abc", "ab", ""
  );
  FAILURE_EQUAL(
      "expected <abc>\n"
      "\tbut was  <ab>\n"
      "\tdifference starts at position 2 at: <        ab          >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualFailureNewVariantAtTheEnd)
{
  mu::tiny::test::StringEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      "EndOfALongerString",
      "EndOfALongerStrinG",
      ""
  );
  FAILURE_EQUAL(
      "expected <EndOfALongerString>\n"
      "\tbut was  <EndOfALongerStrinG>\n"
      "\tdifference starts at position 17 at: <ongerStrinG         >\n"
      "\t                                                ^",
      f
  );
}

TEST(Failure, StringsEqualFailureWithNewLinesAndTabs)
{
  mu::tiny::test::StringEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      "StringWith\t\nDifferentString",
      "StringWith\t\ndifferentString",
      ""
  );

  FAILURE_EQUAL(
      "expected <StringWith\\t\\nDifferentString>\n"
      "\tbut was  <StringWith\\t\\ndifferentString>\n"
      "\tdifference starts at position 12 at: <ngWith\\t\\ndifferentS>\n"
      "\t                                                ^",
      f
  );
}

TEST(Failure, StringsEqualFailureInTheMiddle)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "aa", "ab", ""
  );
  FAILURE_EQUAL(
      "expected <aa>\n"
      "\tbut was  <ab>\n"
      "\tdifference starts at position 1 at: <         ab         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualFailureAtTheBeginning)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "aaa", "bbb", ""
  );
  FAILURE_EQUAL(
      "expected <aaa>\n"
      "\tbut was  <bbb>\n"
      "\tdifference starts at position 0 at: <          bbb       >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualFailureWithNullAsActual)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, "abc", nullptr, ""
  );
  FAILURE_EQUAL(
      "expected <abc>\n"
      "\tbut was  <(null)>",
      f
  );
}

TEST(Failure, StringsEqualFailureWithNullAsExpected)
{
  mu::tiny::test::StringEqualFailure f(
      test, fail_file_name, fail_line_number, nullptr, "abd", ""
  );
  FAILURE_EQUAL(
      "expected <(null)>\n"
      "\tbut was  <abd>",
      f
  );
}

TEST(Failure, StringsEqualNoCaseFailureWithText)
{
  mu::tiny::test::StringEqualNoCaseFailure f(
      test, fail_file_name, fail_line_number, "ABC", "abd", "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <ABC>\n"
      "\tbut was  <abd>\n"
      "\tdifference starts at position 2 at: <        abd         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualNoCaseFailure)
{
  mu::tiny::test::StringEqualNoCaseFailure f(
      test, fail_file_name, fail_line_number, "ABC", "abd", ""
  );
  FAILURE_EQUAL(
      "expected <ABC>\n"
      "\tbut was  <abd>\n"
      "\tdifference starts at position 2 at: <        abd         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, StringsEqualNoCaseFailureWithActualAsNull)
{
  mu::tiny::test::StringEqualNoCaseFailure f(
      test, fail_file_name, fail_line_number, "ABC", nullptr, ""
  );
  FAILURE_EQUAL(
      "expected <ABC>\n"
      "\tbut was  <(null)>",
      f
  );
}

TEST(Failure, StringsEqualNoCaseFailureWithExpectedAsNull)
{
  mu::tiny::test::StringEqualNoCaseFailure f(
      test, fail_file_name, fail_line_number, nullptr, "abd", ""
  );
  FAILURE_EQUAL(
      "expected <(null)>\n"
      "\tbut was  <abd>",
      f
  );
}

TEST(Failure, StringsEqualNoCaseFailure2)
{
  mu::tiny::test::StringEqualNoCaseFailure f(
      test, fail_file_name, fail_line_number, "ac", "AB", ""
  );
  FAILURE_EQUAL(
      "expected <ac>\n"
      "\tbut was  <AB>\n"
      "\tdifference starts at position 1 at: <         AB         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, DoublesEqualNormalWithText)
{
  mu::tiny::test::ApproxEqualFailure<double> f(
      test, fail_file_name, fail_line_number, 1.0, 2.0, 3.0, "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <1>\n"
      "\tbut was  <2> threshold used was <3>",
      f
  );
}

TEST(Failure, DoublesEqualNormal)
{
  mu::tiny::test::ApproxEqualFailure<double> f(
      test, fail_file_name, fail_line_number, 1.0, 2.0, 3.0, ""
  );
  FAILURE_EQUAL(
      "expected <1>\n"
      "\tbut was  <2> threshold used was <3>",
      f
  );
}

TEST(Failure, BinaryEqualWithText)
{
  const unsigned char expected_data[] = { 0x00 };
  const unsigned char actual_data[] = { 0x01 };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      "text"
  );
  FAILURE_EQUAL(
      "Message: text\n"
      "\texpected <00>\n"
      "\tbut was  <01>\n"
      "\tdifference starts at position 0 at: <         01         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualOneByte)
{
  const unsigned char expected_data[] = { 0x00 };
  const unsigned char actual_data[] = { 0x01 };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL(
      "expected <00>\n"
      "\tbut was  <01>\n"
      "\tdifference starts at position 0 at: <         01         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualTwoBytes)
{
  const unsigned char expected_data[] = { 0x00, 0x01 };
  const unsigned char actual_data[] = { 0x00, 0x02 };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL(
      "expected <00 01>\n"
      "\tbut was  <00 02>\n"
      "\tdifference starts at position 1 at: <      00 02         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualThreeBytes)
{
  const unsigned char expected_data[] = { 0x00, 0x01, 0x00 };
  const unsigned char actual_data[] = { 0x00, 0x02, 0x00 };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL(
      "expected <00 01 00>\n"
      "\tbut was  <00 02 00>\n"
      "\tdifference starts at position 1 at: <      00 02 00      >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualFullWidth)
{
  const unsigned char expected_data[] = { 0x00, 0x00, 0x00, 0x01,
                                          0x00, 0x00, 0x00 };
  const unsigned char actual_data[] = {
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
  };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL(
      "expected <00 00 00 01 00 00 00>\n"
      "\tbut was  <00 00 00 02 00 00 00>\n"
      "\tdifference starts at position 3 at: <00 00 00 02 00 00 00>\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualLast)
{
  const unsigned char expected_data[] = { 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00 };
  const unsigned char actual_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
  };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      actual_data,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL(
      "expected <00 00 00 00 00 00 00>\n"
      "\tbut was  <00 00 00 00 00 00 01>\n"
      "\tdifference starts at position 6 at: <00 00 00 01         >\n"
      "\t                                               ^",
      f
  );
}

TEST(Failure, BinaryEqualActualNull)
{
  const unsigned char expected_data[] = { 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00 };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      expected_data,
      nullptr,
      sizeof(expected_data),
      ""
  );
  FAILURE_EQUAL("expected <00 00 00 00 00 00 00>\n\tbut was  <(null)>", f);
}

TEST(Failure, BinaryEqualExpectedNull)
{
  const unsigned char actual_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
  };
  mu::tiny::test::BinaryEqualFailure f(
      test,
      fail_file_name,
      fail_line_number,
      nullptr,
      actual_data,
      sizeof(actual_data),
      ""
  );
  FAILURE_EQUAL("expected <(null)>\n\tbut was  <00 00 00 00 00 00 01>", f);
}

TEST(Failure, FeatureUnsupported)
{
  mu::tiny::test::FeatureUnsupportedFailure f(
      test, fail_file_name, fail_line_number, "SOME_FEATURE", ""
  );
  FAILURE_EQUAL(
      "The feature \"SOME_FEATURE\" is not supported in this "
      "environment or with "
      "the feature set selected when building the library.",
      f
  );
}

TEST(Failure, EqualsFailureWithShortEscapeControlChar)
{
  mu::tiny::test::CheckEqualFailure f(
      test, fail_file_name, fail_line_number, "a\tb", "a\nb", ""
  );
  STRCMP_CONTAINS("\\t", f.get_message().c_str());
  STRCMP_CONTAINS("\\n", f.get_message().c_str());
}

TEST(Failure, EqualsFailureWithHexEscapeControlChar)
{
  mu::tiny::test::CheckEqualFailure f(
      test, fail_file_name, fail_line_number, "a\x01z", "b\x01z", ""
  );
  STRCMP_CONTAINS("\\x01", f.get_message().c_str());
}

#if MUTINY_HAVE_EXCEPTIONS
TEST(Failure, UnexpectedExceptionFailure_UnknownException)
{
  mu::tiny::test::UnexpectedExceptionFailure f(test);
  FAILURE_EQUAL("Unexpected exception of unknown type was thrown.", f);
}
#endif

#if MUTINY_HAVE_EXCEPTIONS && MUTINY_USE_STD_CPP_LIB
TEST(Failure, UnexpectedExceptionFailure_StandardException)
{
  std::runtime_error e("Some error");
  mu::tiny::test::UnexpectedExceptionFailure f(test, e);
#if MUTINY_HAVE_RTTI
  STRCMP_CONTAINS("Unexpected exception of type '", f.get_message().c_str());
  STRCMP_CONTAINS("runtime_error", f.get_message().c_str());
  STRCMP_CONTAINS("' was thrown: Some error", f.get_message().c_str());
#else
  FAILURE_EQUAL("Unexpected exception of unknown type was thrown.", f);
#endif
}
#endif
