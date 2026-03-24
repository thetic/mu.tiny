#include "mutiny/test/Shell.hpp"

#include "mutiny/test/ConsoleOutput.hpp"
#include "mutiny/test/Failure.hpp"
#include "mutiny/test/Plugin.hpp"
#include "mutiny/test/Registry.hpp"
#include "mutiny/test/Result.hpp"
#include "mutiny/test/Test.hpp"
#include "mutiny/test/jump_buffer.h"
#include "mutiny/test/math.hpp"

#include <math.h>
#include <stdlib.h>

namespace mu {
namespace tiny {
namespace test {

namespace {

/* Sometimes stubs use the mutiny assertions.
 * Its not correct to do so, but this small helper class will prevent a
 * segmentation fault and instead will give an error message and also the
 * file/line of the check that was executed outside the tests.
 */
class OutsideTestRunnerUTest : public Shell
{
public:
  static OutsideTestRunnerUTest& instance();
  virtual Result& get_test_result() { return default_test_result_; }
  ~OutsideTestRunnerUTest() override = default;

private:
  OutsideTestRunnerUTest()
    : Shell(
          "\n\t NOTE: Assertion happened without being in a test run "
          "(perhaps in "
          "main?)",
          "\n\t       Something is very wrong. Check this assertion and fix",
          "unknown file",
          0
      )
    , default_test_result_(default_output_)
  {
  }
  ConsoleOutput default_output_;
  Result default_test_result_;
};

OutsideTestRunnerUTest& OutsideTestRunnerUTest::instance()
{
  static OutsideTestRunnerUTest instance;
  return instance;
}

/*
 * Below helpers are used for the PlatformSpecificSetJmp and LongJmp. They pass
 * a method for what needs to happen after the jump, so that the stack stays
 * right.
 *
 */

struct HelperTestRunInfo
{
  HelperTestRunInfo(Shell* s, Plugin* p, Result* r)
    : shell(s)
    , plugin(p)
    , result(r)
  {
  }

  Shell* shell;
  Plugin* plugin;
  Result* result;
};

void helper_do_run_one_test_in_current_process(void* data)
{
  auto* run_info = static_cast<HelperTestRunInfo*>(data);

  Shell* shell = run_info->shell;
  Plugin* plugin = run_info->plugin;
  Result* result = run_info->result;

  shell->run_one_test_in_current_process(plugin, *result);
}

const NormalTerminator normal_test_terminator = NormalTerminator();
const CrashingTerminator crashing_test_terminator = CrashingTerminator();
const TerminatorWithoutExceptions normal_test_terminator_without_exceptions =
    TerminatorWithoutExceptions();
const CrashingTerminatorWithoutExceptions
    crashing_test_terminator_without_exceptions =
        CrashingTerminatorWithoutExceptions();

void (*please_crash_me_right_now)() = abort;
} // namespace

bool doubles_equal(double d1, double d2, double threshold)
{
  if (mu::tiny::test::is_nan(d1) || mu::tiny::test::is_nan(d2) ||
      mu::tiny::test::is_nan(threshold))
    return false;

  if (mu::tiny::test::is_inf(d1) && mu::tiny::test::is_inf(d2)) {
    return true;
  }

  return fabs(d1 - d2) <= threshold;
}

const Terminator* Shell::current_test_terminator_ = &normal_test_terminator;
const Terminator* Shell::current_test_terminator_without_exceptions_ =
    &normal_test_terminator_without_exceptions;

bool Shell::rethrow_exceptions_ = false;

/******************************** */

Shell::Shell()
  : group_("UndefinedTestGroup")
  , name_("UndefinedTest")
  , file_("UndefinedFile")
  , line_number_(0)
  , next_(nullptr)
  , has_failed_(false)
{
}

Shell::Shell(
    const char* group_name,
    const char* test_name,
    const char* file_name,
    size_t line_number
)
  : group_(group_name)
  , name_(test_name)
  , file_(file_name)
  , line_number_(line_number)
  , next_(nullptr)
  , has_failed_(false)
{
}

void Shell::set_crash_method(void (*crashme)())
{
  please_crash_me_right_now = crashme;
}

void Shell::reset_crash_method()
{
  please_crash_me_right_now = abort;
}

void Shell::crash()
{
  please_crash_me_right_now();
}

void Shell::run_one_test(Plugin* plugin, Result& result)
{
  has_failed_ = false;
  result.count_run();
  HelperTestRunInfo run_info(this, plugin, &result);
  mutiny_set_jmp(helper_do_run_one_test_in_current_process, &run_info);
}

Test* Shell::create_test()
{
  return new Test();
}

void Shell::destroy_test(Test* test)
{
  delete test;
}

void Shell::run_one_test_in_current_process(Plugin* plugin, Result& result)
{
  result.print_very_verbose("\n-- before runAllPreTestAction: ");
  plugin->run_all_pre_test_action(*this, result);
  result.print_very_verbose("\n-- after runAllPreTestAction: ");

  // save test context, so that test class can be tested
  Shell* saved_test = Shell::get_current();
  Result* saved_result = Shell::get_test_result();

  Shell::set_test_result(&result);
  Shell::set_current_test(this);

  Test* test_to_run = nullptr;

#if MUTINY_HAVE_EXCEPTIONS
  try {
#endif
    result.print_very_verbose("\n---- before createTest: ");
    test_to_run = create_test();
    result.print_very_verbose("\n---- after createTest: ");

    result.print_very_verbose("\n------ before runTest: ");
    test_to_run->run();
    result.print_very_verbose("\n------ after runTest: ");

    Shell::set_current_test(saved_test);
    Shell::set_test_result(saved_result);
#if MUTINY_HAVE_EXCEPTIONS
  } catch (...) {
    destroy_test(test_to_run);
    throw;
  }
#endif

  result.print_very_verbose("\n---- before destroyTest: ");
  destroy_test(test_to_run);
  result.print_very_verbose("\n---- after destroyTest: ");

  result.print_very_verbose("\n-- before runAllPostTestAction: ");
  plugin->run_all_post_test_action(*this, result);
  result.print_very_verbose("\n-- after runAllPostTestAction: ");
}

Shell* Shell::get_next() const
{
  return next_;
}

Shell* Shell::add_test(Shell* test)
{
  next_ = test;
  return this;
}

size_t Shell::count_tests()
{
  return next_ ? next_->count_tests() + 1 : 1;
}

String Shell::get_macro_name() const
{
  return "TEST";
}

const char* Shell::get_name() const
{
  return name_;
}

const char* Shell::get_group() const
{
  return group_;
}

String Shell::get_formatted_name() const
{
  String formatted_name(get_macro_name());
  formatted_name += "(";
  formatted_name += group_;
  formatted_name += ", ";
  formatted_name += name_;
  formatted_name += ")";

  return formatted_name;
}

bool Shell::has_failed() const
{
  return has_failed_;
}

void Shell::count_check()
{
  get_test_result()->count_check();
}

bool Shell::will_run() const
{
  return true;
}

bool Shell::is_ordered() const
{
  return false;
}

void Shell::set_run_ignored() {}

void Shell::set_file_name(const char* file_name)
{
  file_ = file_name;
}

void Shell::set_line_number(size_t line_number)
{
  line_number_ = line_number;
}

void Shell::set_group_name(const char* group_name)
{
  group_ = group_name;
}

void Shell::set_test_name(const char* test_name)
{
  name_ = test_name;
}

const char* Shell::get_file() const
{
  return file_;
}

size_t Shell::get_line_number() const
{
  return line_number_;
}

bool Shell::match(const char* target, const Filter* filters) const
{
  if (filters == nullptr)
    return true;

  for (; filters != nullptr; filters = filters->get_next())
    if (filters->match(target))
      return true;

  return false;
}

bool Shell::should_run(
    const Filter* group_filters,
    const Filter* name_filters
) const
{
  return match(group_, group_filters) && match(name_, name_filters);
}

void Shell::add_failure(const Failure& failure)
{
  has_failed_ = true;
  get_test_result()->add_failure(failure);
}

void Shell::add_test_property(const char* name, const char* value)
{
  get_test_result()->add_test_property(name, value);
}

void Shell::exit_test(const Terminator& terminator)
{
  terminator.exit_current_test();
}

void Shell::assert_true(
    bool condition,
    const char* check_string,
    const char* condition_string,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (!condition) {
    add_failure(CheckFailure(
        this, file_name, line_number, check_string, condition_string, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::fail(
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  add_failure(FailFailure(this, file_name, line_number, text));
  test_terminator.exit_current_test();
}

void Shell::assert_cstr_equal(
    const char* expected,
    const char* actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (actual == nullptr && expected == nullptr)
    return;
  if (actual == nullptr || expected == nullptr) {
    add_failure(
        StringEqualFailure(this, file_name, line_number, expected, actual, text)
    );
    test_terminator.exit_current_test();
  }
  if (strcmp(expected, actual) != 0) {
    add_failure(
        StringEqualFailure(this, file_name, line_number, expected, actual, text)
    );
    test_terminator.exit_current_test();
  }
}

void Shell::assert_cstr_n_equal(
    const char* expected,
    const char* actual,
    size_t length,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (actual == nullptr && expected == nullptr)
    return;
  if (actual == nullptr || expected == nullptr) {
    add_failure(
        StringEqualFailure(this, file_name, line_number, expected, actual, text)
    );
    test_terminator.exit_current_test();
  }
  if (strncmp(expected, actual, length) != 0) {
    add_failure(
        StringEqualFailure(this, file_name, line_number, expected, actual, text)
    );
    test_terminator.exit_current_test();
  }
}

void Shell::assert_cstr_contains(
    const char* expected,
    const char* actual,
    const char* text,
    const char* file_name,
    size_t line_number
)
{
  get_test_result()->count_check();
  if (actual == nullptr && expected == nullptr)
    return;
  if (actual == nullptr || expected == nullptr) {
    add_failure(ContainsFailure(
        this,
        file_name,
        line_number,
        string_from_or_null(expected),
        string_from_or_null(actual),
        text ? text : ""
    ));
    exit_test(get_current_test_terminator());
  } else if (!string_contains(actual, expected)) {
    add_failure(ContainsFailure(
        this, file_name, line_number, expected, actual, text ? text : ""
    ));
    exit_test(get_current_test_terminator());
  }
}

void Shell::assert_longs_equal(
    long expected,
    long actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(
        LongsEqualFailure(this, file_name, line_number, expected, actual, text)
    );
    test_terminator.exit_current_test();
  }
}

void Shell::assert_unsigned_longs_equal(
    unsigned long expected,
    unsigned long actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(UnsignedLongsEqualFailure(
        this, file_name, line_number, expected, actual, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_long_longs_equal(
    long long expected,
    long long actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(LongLongsEqualFailure(
        this, file_name, line_number, expected, actual, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_unsigned_long_longs_equal(
    unsigned long long expected,
    unsigned long long actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(UnsignedLongLongsEqualFailure(
        this, file_name, line_number, expected, actual, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_signed_bytes_equal(
    signed char expected,
    signed char actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(SignedBytesEqualFailure(
        this, file_name, line_number, expected, actual, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_pointers_equal(
    const void* expected,
    const void* actual,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(EqualsFailure(
        this,
        file_name,
        line_number,
        string_from(expected),
        string_from(actual),
        text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_function_pointers_equal(
    void (*expected)(),
    void (*actual)(),
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (expected != actual) {
    add_failure(EqualsFailure(
        this,
        file_name,
        line_number,
        string_from(expected),
        string_from(actual),
        text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_doubles_equal(
    double expected,
    double actual,
    double threshold,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (!doubles_equal(expected, actual, threshold)) {
    add_failure(DoublesEqualFailure(
        this, file_name, line_number, expected, actual, threshold, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_binary_equal(
    const void* expected,
    const void* actual,
    size_t length,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (length == 0)
    return;
  if (actual == nullptr && expected == nullptr)
    return;
  if (actual == nullptr || expected == nullptr) {
    add_failure(BinaryEqualFailure(
        this,
        file_name,
        line_number,
        static_cast<const unsigned char*>(expected),
        static_cast<const unsigned char*>(actual),
        length,
        text
    ));
    test_terminator.exit_current_test();
  }
  if (memcmp(expected, actual, length) != 0) {
    add_failure(BinaryEqualFailure(
        this,
        file_name,
        line_number,
        static_cast<const unsigned char*>(expected),
        static_cast<const unsigned char*>(actual),
        length,
        text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_equals(
    bool failed,
    const char* expected,
    const char* actual,
    const char* text,
    const char* file,
    size_t line,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (failed) {
    add_failure(CheckEqualFailure(this, file, line, expected, actual, text));
    test_terminator.exit_current_test();
  }
}

void Shell::assert_equals(
    bool failed,
    String expected,
    String actual,
    const char* text,
    const char* file,
    size_t line,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (failed) {
    add_failure(CheckEqualFailure(
        this, file, line, expected.c_str(), actual.c_str(), text
    ));
    expected.clear();
    actual.clear();
    test_terminator.exit_current_test();
  }
}

void Shell::assert_compare(
    bool comparison,
    const char* check_string,
    const char* comparison_string,
    const char* text,
    const char* file_name,
    size_t line_number,
    const Terminator& test_terminator
)
{
  get_test_result()->count_check();
  if (!comparison) {
    add_failure(ComparisonFailure(
        this, file_name, line_number, check_string, comparison_string, text
    ));
    test_terminator.exit_current_test();
  }
}

void Shell::print_very_verbose(const char* text)
{
  get_test_result()->print_very_verbose(text);
}

Result* Shell::test_result_ = nullptr;
Shell* Shell::current_test_ = nullptr;

void Shell::set_test_result(Result* result)
{
  test_result_ = result;
}

void Shell::set_current_test(Shell* test)
{
  current_test_ = test;
}

Result* Shell::get_test_result()
{
  if (test_result_ == nullptr)
    return &OutsideTestRunnerUTest::instance().get_test_result();
  return test_result_;
}

Shell* Shell::get_current()
{
  if (current_test_ == nullptr)
    return &OutsideTestRunnerUTest::instance();
  return current_test_;
}

const Terminator& Shell::get_current_test_terminator()
{
  return *current_test_terminator_;
}

const Terminator& Shell::get_current_test_terminator_without_exceptions()
{
  return *current_test_terminator_without_exceptions_;
}

void Shell::set_crash_on_fail()
{
  current_test_terminator_ = &crashing_test_terminator;
  current_test_terminator_without_exceptions_ =
      &crashing_test_terminator_without_exceptions;
}

void Shell::restore_default_test_terminator()
{
  current_test_terminator_ = &normal_test_terminator;
  current_test_terminator_without_exceptions_ =
      &normal_test_terminator_without_exceptions;
}

void Shell::set_rethrow_exceptions(bool rethrow_exceptions)
{
  rethrow_exceptions_ = rethrow_exceptions;
}

bool Shell::is_rethrowing_exceptions()
{
  return rethrow_exceptions_;
}

} // namespace test
} // namespace tiny
} // namespace mu
