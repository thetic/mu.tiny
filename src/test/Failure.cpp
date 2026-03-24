#include "mutiny/test/Failure.hpp"

#include "mutiny/test/Output.hpp"
#include "mutiny/test/Shell.hpp"
#include "mutiny/test/String.hpp"
#include "mutiny/test/math.hpp"

#if MUTINY_USE_STD_CPP_LIB
#include <typeinfo>
#if defined(__GNUC__)
#include <memory>

#include <cxxabi.h>
#endif
#endif

namespace mu {
namespace tiny {
namespace test {
namespace {
size_t get_printable_size(String const& str)
{
  size_t str_size = str.size();
  size_t printable_str_size = str_size;

  for (size_t i = 0; i < str_size; i++) {
    char c = str.c_str()[i];
    if (is_control_with_short_escape_sequence(c)) {
      printable_str_size += 1;
    } else if (iscntrl(c)) {
      printable_str_size += 3;
    }
  }

  return printable_str_size;
}

String printable(String const& str)
{
  static const char* short_escape_codes[] = { "\\a", "\\b", "\\t", "\\n",
                                              "\\v", "\\f", "\\r" };

  String result;
  result.reserve(get_printable_size(str));

  size_t str_size = str.size();
  for (size_t i = 0; i < str_size; i++) {
    char c = str.c_str()[i];
    if (is_control_with_short_escape_sequence(c)) {
      result += short_escape_codes[static_cast<unsigned char>(c - '\a')];
    } else if (iscntrl(c)) {
      result += string_from_format("\\x%02X ", c);
    } else {
      result += c;
    }
  }

  return result;
}

String printable_string_from_or_null(const char* expected)
{
  return (expected) ? printable(string_from(expected)) : string_from("(null)");
}
} // namespace

Failure::Failure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& the_message
)
  : test_name_(test->get_formatted_name())
  , test_name_only_(test->get_name())
  , file_name_(file_name)
  , line_number_(line_number)
  , test_file_name_(test->get_file())
  , test_line_number_(test->get_line_number())
  , message_(the_message)
{
}

Failure::Failure(Shell* test, const String& the_message)
  : test_name_(test->get_formatted_name())
  , test_name_only_(test->get_name())
  , file_name_(test->get_file())
  , line_number_(test->get_line_number())
  , test_file_name_(test->get_file())
  , test_line_number_(test->get_line_number())
  , message_(the_message)
{
}

Failure::Failure(Shell* test, const char* file_name, size_t line_num)
  : test_name_(test->get_formatted_name())
  , test_name_only_(test->get_name())
  , file_name_(file_name)
  , line_number_(line_num)
  , test_file_name_(test->get_file())
  , test_line_number_(test->get_line_number())
  , message_("no message")
{
}

Failure::Failure(Failure&& f) noexcept
  : test_name_(static_cast<String&&>(f.test_name_))
  , test_name_only_(static_cast<String&&>(f.test_name_only_))
  , file_name_(static_cast<String&&>(f.file_name_))
  , line_number_(f.line_number_)
  , test_file_name_(static_cast<String&&>(f.test_file_name_))
  , test_line_number_(f.test_line_number_)
  , message_(static_cast<String&&>(f.message_))
{
}

const String& Failure::get_file_name() const
{
  return file_name_;
}

const String& Failure::get_test_file_name() const
{
  return test_file_name_;
}

const String& Failure::get_test_name() const
{
  return test_name_;
}

const String& Failure::get_test_name_only() const
{
  return test_name_only_;
}

size_t Failure::get_failure_line_number() const
{
  return line_number_;
}

size_t Failure::get_test_line_number() const
{
  return test_line_number_;
}

const String& Failure::get_message() const
{
  return message_;
}

bool Failure::is_outside_test_file() const
{
  return test_file_name_ != file_name_;
}

bool Failure::is_in_helper_function() const
{
  return line_number_ < test_line_number_;
}

String Failure::create_but_was_string(
    const String& expected,
    const String& actual
)
{
  return string_from_format(
      "expected <%s>\n\tbut was  <%s>", expected.c_str(), actual.c_str()
  );
}

String Failure::create_difference_at_pos_string(
    const String& actual,
    size_t offset,
    size_t reported_position
)
{
  String result;
  const size_t extra_characters_window = 20;
  const size_t half_of_extra_characters_window = extra_characters_window / 2;

  String padding_for_preventing_out_of_bounds(
      half_of_extra_characters_window, ' '
  );
  String actual_string = padding_for_preventing_out_of_bounds + actual +
                         padding_for_preventing_out_of_bounds;
  String different_string = string_from_format(
      "difference starts at position %lu at: <",
      static_cast<unsigned long>(reported_position)
  );

  result += "\n";
  result += string_from_format(
      "\t%s%s>\n",
      different_string.c_str(),
      actual_string.substr(offset, extra_characters_window).c_str()
  );

  result += string_from_format(
      "\t%s^",
      String(different_string.size() + half_of_extra_characters_window, ' ')
          .c_str()
  );
  return result;
}

String Failure::create_user_text(const String& text)
{
  String user_message = "";
  if (!text.empty()) {
    // This is a kludge to turn off "Message: " for this case.
    // I don't think "Message: " adds anything, as you get to see the
    // message. I propose we remove "Message: " lead in
    if (!string_starts_with(text, "LONGS_EQUAL"))
      user_message += "Message: ";
    user_message += text;
    user_message += "\n\t";
  }
  return user_message;
}

EqualsFailure::EqualsFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const char* expected,
    const char* actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ += create_but_was_string(
      string_from_or_null(expected), string_from_or_null(actual)
  );
}

EqualsFailure::EqualsFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& expected,
    const String& actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ += create_but_was_string(expected, actual);
}

DoublesEqualFailure::DoublesEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    double expected,
    double actual,
    double threshold,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ +=
      create_but_was_string(string_from(expected, 7), string_from(actual, 7));
  message_ += " threshold used was <";
  message_ += string_from(threshold, 7);
  message_ += ">";

  if (mu::tiny::test::is_nan(expected) || mu::tiny::test::is_nan(actual) ||
      mu::tiny::test::is_nan(threshold))
    message_ += "\n\tCannot make comparisons with Nan";
}

CheckEqualFailure::CheckEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& expected,
    const String& actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String printable_expected = printable_string_from_or_null(expected.c_str());
  String printable_actual = printable_string_from_or_null(actual.c_str());

  message_ += create_but_was_string(printable_expected, printable_actual);

  size_t fail_start;
  for (fail_start = 0; actual[fail_start] == expected[fail_start]; fail_start++)
    ;
  size_t fail_start_printable;
  for (fail_start_printable = 0; printable_actual[fail_start_printable] ==
                                 printable_expected[fail_start_printable];
       fail_start_printable++)
    ;
  message_ += create_difference_at_pos_string(
      printable_actual, fail_start_printable, fail_start
  );
}

ComparisonFailure::ComparisonFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& check_string,
    const String& comparison_string,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);
  message_ += check_string;
  message_ += "(";
  message_ += comparison_string;
  message_ += ") failed";
}

ContainsFailure::ContainsFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& expected,
    const String& actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ += string_from_format(
      "actual <%s>\n\tdid not contain  <%s>", actual.c_str(), expected.c_str()
  );
}

CheckFailure::CheckFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& check_string,
    const String& condition_string,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ += check_string;
  message_ += "(";
  message_ += condition_string;
  message_ += ") failed";
}

FailFailure::FailFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& message
)
  : Failure(test, file_name, line_number)
{
  message_ = message;
}

LongsEqualFailure::LongsEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    long expected,
    long actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String a_decimal = string_from(actual);
  String e_decimal = string_from(expected);

  pad_strings_to_same_length(a_decimal, e_decimal, ' ');

  String actual_reported =
      a_decimal + " " + brackets_formatted_hex_string_from(actual);
  String expected_reported =
      e_decimal + " " + brackets_formatted_hex_string_from(expected);
  message_ += create_but_was_string(expected_reported, actual_reported);
}

UnsignedLongsEqualFailure::UnsignedLongsEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    unsigned long expected,
    unsigned long actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String a_decimal = string_from(actual);
  String e_decimal = string_from(expected);

  pad_strings_to_same_length(a_decimal, e_decimal, ' ');

  String actual_reported =
      a_decimal + " " + brackets_formatted_hex_string_from(actual);
  String expected_reported =
      e_decimal + " " + brackets_formatted_hex_string_from(expected);

  message_ += create_but_was_string(expected_reported, actual_reported);
}

LongLongsEqualFailure::LongLongsEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    long long expected,
    long long actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String a_decimal = string_from(actual);
  String e_decimal = string_from(expected);

  pad_strings_to_same_length(a_decimal, e_decimal, ' ');

  String actual_reported =
      a_decimal + " " + brackets_formatted_hex_string_from(actual);
  String expected_reported =
      e_decimal + " " + brackets_formatted_hex_string_from(expected);
  message_ += create_but_was_string(expected_reported, actual_reported);
}

UnsignedLongLongsEqualFailure::UnsignedLongLongsEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    unsigned long long expected,
    unsigned long long actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String a_decimal = string_from(actual);
  String e_decimal = string_from(expected);

  pad_strings_to_same_length(a_decimal, e_decimal, ' ');

  String actual_reported =
      a_decimal + " " + brackets_formatted_hex_string_from(actual);
  String expected_reported =
      e_decimal + " " + brackets_formatted_hex_string_from(expected);
  message_ += create_but_was_string(expected_reported, actual_reported);
}

SignedBytesEqualFailure::SignedBytesEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    signed char expected,
    signed char actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String a_decimal = string_from(static_cast<int>(actual));
  String e_decimal = string_from(static_cast<int>(expected));

  pad_strings_to_same_length(a_decimal, e_decimal, ' ');

  String actual_reported =
      a_decimal + " " + brackets_formatted_hex_string_from(actual);
  String expected_reported =
      e_decimal + " " + brackets_formatted_hex_string_from(expected);
  message_ += create_but_was_string(expected_reported, actual_reported);
}

StringEqualFailure::StringEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const char* expected,
    const char* actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String printable_expected = printable_string_from_or_null(expected);
  String printable_actual = printable_string_from_or_null(actual);

  message_ += create_but_was_string(printable_expected, printable_actual);
  if ((expected) && (actual)) {
    size_t fail_start;
    for (fail_start = 0; actual[fail_start] == expected[fail_start];
         fail_start++)
      ;
    size_t fail_start_printable;
    for (fail_start_printable = 0; printable_actual[fail_start_printable] ==
                                   printable_expected[fail_start_printable];
         fail_start_printable++)
      ;
    message_ += create_difference_at_pos_string(
        printable_actual, fail_start_printable, fail_start
    );
  }
}

StringEqualNoCaseFailure::StringEqualNoCaseFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const char* expected,
    const char* actual,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String printable_expected = printable_string_from_or_null(expected);
  String printable_actual = printable_string_from_or_null(actual);

  message_ += create_but_was_string(printable_expected, printable_actual);
  if ((expected) && (actual)) {
    size_t fail_start;
    for (fail_start = 0;
         tolower(actual[fail_start]) == tolower(expected[fail_start]);
         fail_start++)
      ;
    size_t fail_start_printable;
    for (fail_start_printable = 0;
         tolower(printable_actual[fail_start_printable]) ==
         tolower(printable_expected[fail_start_printable]);
         fail_start_printable++)
      ;
    message_ += create_difference_at_pos_string(
        printable_actual, fail_start_printable, fail_start
    );
  }
}

BinaryEqualFailure::BinaryEqualFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const unsigned char* expected,
    const unsigned char* actual,
    size_t size,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  String actual_hex = string_from_binary_or_null(actual, size);

  message_ += create_but_was_string(
      string_from_binary_or_null(expected, size), actual_hex
  );
  if ((expected) && (actual)) {
    size_t fail_start;
    for (fail_start = 0; actual[fail_start] == expected[fail_start];
         fail_start++)
      ;
    message_ += create_difference_at_pos_string(
        actual_hex, (fail_start * 3 + 1), fail_start
    );
  }
}

FeatureUnsupportedFailure::FeatureUnsupportedFailure(
    Shell* test,
    const char* file_name,
    size_t line_number,
    const String& feature_name,
    const String& text
)
  : Failure(test, file_name, line_number)
{
  message_ = create_user_text(text);

  message_ += string_from_format(
      "The feature \"%s\" is not supported in this environment or with the "
      "feature set selected when building the library.",
      feature_name.c_str()
  );
}

#if MUTINY_HAVE_EXCEPTIONS
UnexpectedExceptionFailure::UnexpectedExceptionFailure(Shell* test)
  : Failure(test, "Unexpected exception of unknown type was thrown.")
{
}

#if MUTINY_USE_STD_CPP_LIB
#if MUTINY_HAVE_RTTI
namespace {
String get_exception_type_name(const std::exception& e)
{
  const char* name = typeid(e).name();
#if defined(__GNUC__)
  int status = -1;

  std::unique_ptr<char, void (*)(void*)> demangled_name(
      abi::__cxa_demangle(name, nullptr, nullptr, &status), free
  );

  return (status == 0) ? demangled_name.get() : name;
#else
  return name;
#endif
}
} // namespace
#endif // MUTINY_HAVE_RTTI

UnexpectedExceptionFailure::UnexpectedExceptionFailure(
    Shell* test,
    const std::exception& e
)
  : Failure(
        test,
#if MUTINY_HAVE_RTTI
        string_from_format(
            "Unexpected exception of type '%s' was thrown: %s",
            get_exception_type_name(e).c_str(),
            e.what()
        )
#else
        "Unexpected exception of unknown type was thrown."
#endif
    )
{
  (void)e;
}
#endif // MUTINY_USE_STD_CPP_LIB
#endif // MUTINY_HAVE_EXCEPTIONS

} // namespace test
} // namespace tiny
} // namespace mu
