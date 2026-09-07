#include "mu/tiny/test/Failure.hpp"
#include "mu/tiny/test/JUnitOutputPlugin.hpp"
#include "mu/tiny/test/Output.hpp"
#include "mu/tiny/test/Result.hpp"
#include "mu/tiny/test/Shell.hpp"
#include "mu/tiny/test/SkippedShell.hpp"

#include "mu/tiny/String.hpp"
#include "mu/tiny/test.hpp"
#include "mu/tiny/time.hpp"

namespace {

class FileForJUnitTestOutputs
{
  mu::tiny::String name_;
  bool is_open_{ true };
  mu::tiny::String buffer_;
  mu::tiny::String line_buf_;
  FileForJUnitTestOutputs* next_;

public:
  FileForJUnitTestOutputs(
      const mu::tiny::String& filename,
      FileForJUnitTestOutputs* next
  )
    : name_(filename)
    , next_(next)
  {
  }

  FileForJUnitTestOutputs* next_file() { return next_; }

  mu::tiny::String name() { return name_; }

  void write(const char* buf) { buffer_ += buf; }

  void close() { is_open_ = false; }

  const char* line(size_t n)
  {
    const char* p = buffer_.c_str();
    size_t count = 1;
    while ((*p != 0) && count < n) {
      if (*p == '\n') {
        ++count;
      }
      ++p;
    }
    line_buf_ = "";
    while ((*p != 0) && *p != '\n') {
      line_buf_ += *p++;
    }
    if (*p == '\n') {
      line_buf_ += '\n';
    }
    return line_buf_.c_str();
  }

  const char* line_from_the_back(size_t n)
  {
    return line(amount_of_lines() - (n - 1));
  }

  size_t amount_of_lines()
  {
    size_t count = 0;
    for (const char* p = buffer_.c_str(); *p != 0; ++p) {
      if (*p == '\n') {
        ++count;
      }
    }
    return count;
  }

  mu::tiny::String content() { return buffer_; }
};

class FileSystemForJUnitTestOutputTests
{
  FileForJUnitTestOutputs* first_file_{ nullptr };

public:
  FileSystemForJUnitTestOutputTests() = default;
  ~FileSystemForJUnitTestOutputTests() { clear(); }

  void clear()
  {
    while (first_file_ != nullptr) {
      FileForJUnitTestOutputs* tmp = first_file_;
      first_file_ = first_file_->next_file();
      delete tmp;
    }
  }

  FileForJUnitTestOutputs* open_file(const mu::tiny::String& filename)
  {
    first_file_ = new FileForJUnitTestOutputs(filename, first_file_);
    return first_file_;
  }

  int amount_of_files()
  {
    int count = 0;
    for (auto* cur = first_file_; cur != nullptr; cur = cur->next_file()) {
      ++count;
    }
    return count;
  }

  bool file_exists(const char* filename) { return file(filename) != nullptr; }

  FileForJUnitTestOutputs* file(const char* filename)
  {
    for (auto* cur = first_file_; cur != nullptr; cur = cur->next_file()) {
      if (cur->name() == filename) {
        return cur;
      }
    }
    return nullptr;
  }
};

uint_least64_t millis_time = 0;
const char* the_time = "";

uint_least64_t mock_get_time_in_millis()
{
  return millis_time;
}

const char* mock_get_time_string()
{
  return the_time;
}

struct PendingProperty
{
  const char* name;
  const char* value;
  PendingProperty* next{ nullptr };
};

class JUnitTestOutputTestRunner
{
  mu::tiny::test::Result result_;

  const char* current_group_name_{ nullptr };
  mu::tiny::test::Shell* current_test_{ nullptr };
  bool first_test_in_group_{ true };
  unsigned int time_the_test_takes_{ 0 };
  unsigned int number_of_checks_in_test_{ 0 };
  mu::tiny::test::Failure* test_failure_{ nullptr };
  const char* pending_skip_message_{ nullptr };
  PendingProperty* pending_properties_{ nullptr };

public:
  explicit JUnitTestOutputTestRunner(const mu::tiny::test::Result& result)
    : result_(result)
  {
    millis_time = 0;
    the_time = "1978-10-03T00:00:00";
    MUTINY_PTR_SET(mu::tiny::get_time_in_millis, mock_get_time_in_millis);
    MUTINY_PTR_SET(mu::tiny::get_time_string, mock_get_time_string);
  }

  JUnitTestOutputTestRunner& start()
  {
    result_.tests_started();
    return *this;
  }

  JUnitTestOutputTestRunner& end()
  {
    end_of_previous_test_group();
    delete current_test_;
    result_.tests_ended();
    return *this;
  }

  JUnitTestOutputTestRunner& end_group_and_clear_test()
  {
    end_of_previous_test_group();
    delete current_test_;
    current_test_ = nullptr;
    return *this;
  }

  void end_of_previous_test_group()
  {
    run_previous_test();
    if (current_test_ != nullptr) {
      result_.current_group_ended(current_test_);
      first_test_in_group_ = true;
    }
    current_group_name_ = nullptr;
  }

  JUnitTestOutputTestRunner& with_group(const char* group_name)
  {
    end_group_and_clear_test();
    current_group_name_ = group_name;
    return *this;
  }

  JUnitTestOutputTestRunner& with_test(const char* test_name)
  {
    run_previous_test();
    delete current_test_;
    current_test_ =
        new mu::tiny::test::Shell(current_group_name_, test_name, "file", 1);
    return *this;
  }

  JUnitTestOutputTestRunner& with_skipped_test(const char* test_name)
  {
    run_previous_test();
    delete current_test_;
    current_test_ = new mu::tiny::test::SkippedShell(
        current_group_name_, test_name, "file", 1
    );
    return *this;
  }

  JUnitTestOutputTestRunner& in_file(const char* file_name)
  {
    if (current_test_ != nullptr) {
      current_test_->set_file_name(file_name);
    }
    return *this;
  }

  JUnitTestOutputTestRunner& on_line(int_least32_t line_number)
  {
    if (current_test_ != nullptr) {
      current_test_->set_line_number(line_number);
    }
    return *this;
  }

  void run_previous_test()
  {
    if (current_test_ == nullptr) {
      return;
    }

    if (first_test_in_group_) {
      result_.current_group_started(current_test_);
      first_test_in_group_ = false;
    }
    result_.current_test_started(current_test_);

    millis_time += time_the_test_takes_;
    for (unsigned int i = 0; i < number_of_checks_in_test_; ++i) {
      result_.count_check();
    }
    number_of_checks_in_test_ = 0;

    while (pending_properties_ != nullptr) {
      result_.add_test_property(
          pending_properties_->name, pending_properties_->value
      );
      PendingProperty* tmp = pending_properties_->next;
      delete pending_properties_;
      pending_properties_ = tmp;
    }

    if (pending_skip_message_ != nullptr) {
      result_.skip_test(pending_skip_message_);
      pending_skip_message_ = nullptr;
    }

    if (test_failure_ != nullptr) {
      result_.add_failure(*test_failure_);
      delete test_failure_;
      test_failure_ = nullptr;
    }

    result_.current_test_ended(current_test_);
  }

  JUnitTestOutputTestRunner& that_has_checks(unsigned int num)
  {
    number_of_checks_in_test_ = num;
    return *this;
  }

  JUnitTestOutputTestRunner& that_takes(unsigned int time_elapsed)
  {
    time_the_test_takes_ = time_elapsed;
    return *this;
  }

  JUnitTestOutputTestRunner& seconds() { return *this; }

  JUnitTestOutputTestRunner& that_fails(
      const char* message,
      const char* file,
      int_least32_t line
  )
  {
    test_failure_ =
        new mu::tiny::test::Failure(current_test_, file, line, message);
    return *this;
  }

#if MUTINY_HAVE_EXCEPTIONS
  JUnitTestOutputTestRunner& that_errors()
  {
    test_failure_ =
        // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
        new mu::tiny::test::UnexpectedExceptionFailure(current_test_);
    return *this;
  }
#endif

  JUnitTestOutputTestRunner& at_time(const char* new_time)
  {
    the_time = new_time;
    return *this;
  }

  JUnitTestOutputTestRunner& that_prints(const char* output)
  {
    run_previous_test();
    result_.print(output);
    return *this;
  }

  JUnitTestOutputTestRunner& that_is_skipped(const char* message)
  {
    pending_skip_message_ = message;
    return *this;
  }

  JUnitTestOutputTestRunner& with_property(const char* name, const char* value)
  {
    auto* prop = new PendingProperty;
    prop->name = name;
    prop->value = value;
    prop->next = nullptr;
    PendingProperty** tail = &pending_properties_;
    while (*tail != nullptr) {
      tail = &(*tail)->next;
    }
    *tail = prop;
    return *this;
  }
};

FileSystemForJUnitTestOutputTests file_system;
FileForJUnitTestOutputs* current_file = nullptr;

mu::tiny::test::Output::File mock_f_open(
    const char* filename,
    const char* /*flag*/
)
{
  current_file = file_system.open_file(filename);
  return current_file;
}

mu::tiny::test::Output::FPutsFunc original_f_puts;

void mock_f_puts(const char* str, mu::tiny::test::Output::File file)
{
  if (file == current_file) {
    static_cast<FileForJUnitTestOutputs*>(file)->write(str);
  } else {
    original_f_puts(str, file);
  }
}

void mock_f_close(mu::tiny::test::Output::File file)
{
  current_file = nullptr;
  static_cast<FileForJUnitTestOutputs*>(file)->close();
}

} // namespace

TEST_GROUP(JUnitOutput)
{
  mu::tiny::test::JUnitOutputPlugin* plugin;
  mu::tiny::test::Output* output;
  mu::tiny::test::Result* result;
  JUnitTestOutputTestRunner* test_case_runner;
  FileForJUnitTestOutputs* output_file;

  // Recreate the JUnit output with the given -pjunit argument.
  // Use "-pjunit" (no package) for "mutiny.xml"; "-pjunit=name" for "name.xml".
  void init(
      const char* junit_arg = "-pjunit",
      const char* default_package_name = ""
  )
  {
    delete test_case_runner;
    delete result;
    delete output;
    delete plugin;
    plugin = new mu::tiny::test::JUnitOutputPlugin(default_package_name);
    const char* argv[] = { junit_arg };
    plugin->parse_arguments(1, argv);
    output = plugin->create_output();
    result = new mu::tiny::test::Result(*output);
    test_case_runner = new JUnitTestOutputTestRunner(*result);
  }

  void setup() override
  {
    MUTINY_PTR_SET(mu::tiny::test::Output::fopen_, mock_f_open);
    original_f_puts = mu::tiny::test::Output::fputs_;
    MUTINY_PTR_SET(mu::tiny::test::Output::fputs_, mock_f_puts);
    MUTINY_PTR_SET(mu::tiny::test::Output::fclose_, mock_f_close);
    plugin = nullptr;
    output = nullptr;
    result = nullptr;
    test_case_runner = nullptr;
    init();
  }

  void teardown() override
  {
    delete test_case_runner;
    delete result;
    delete output;
    delete plugin;
    file_system.clear();
  }
};

TEST(JUnitOutput, withOneTestGroupAndOneTestOnlyWriteToOneFile)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  CHECK_EQUAL(1, file_system.amount_of_files());
  CHECK(file_system.file_exists("mutiny.xml"));
}

TEST(JUnitOutput, withReservedCharactersInPackageNameUsesUnderscoresForFileName)
{
  init("-pjunit=p/a\\c?k%a*g:e|n\"a<m>e.");
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  CHECK(file_system.file_exists("p_a_c_k_a_g_e_n_a_m_e..xml"));
}

TEST(JUnitOutput, withOneTestGroupAndOneTestOutputsValidXMLFiles)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n", output_file->line(1)
  );
}

TEST(JUnitOutput, withOneTestGroupAndOneTestOutputsTestSuiteStartAndEndBlocks)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuites tests=\"1\" failures=\"0\" errors=\"0\" skipped=\"0\" "
      "time=\"0.000\" timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(2)
  );
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"groupname\" tests=\"1\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL("</testsuite>\n", output_file->line_from_the_back(2));
  STRCMP_EQUAL("</testsuites>\n", output_file->line_from_the_back(1));
}

TEST(JUnitOutput, withOneTestGroupAndOneTestFileHasNoSuiteLevelPropertiesBlock)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, withOneTestGroupAndOneTestFileShouldContainsATestCaseBlock)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
}

TEST(JUnitOutput, withOneTestGroupAndTwoTestCasesCreateCorrectTestgroupBlockAndCorrectTestCaseBlock)
{
  test_case_runner->start()
      .with_group("twoTestsGroup")
      .with_test("firstTestName")
      .with_test("secondTestName")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"twoTestsGroup\" tests=\"2\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"firstTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"secondTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(6)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(7));
}

TEST(JUnitOutput, withOneTestGroupAndTimeHasElapsedAndTimestampChanged)
{
  test_case_runner->start()
      .at_time("2013-07-04T22:28:00")
      .with_group("timeGroup")
      .with_test("Dummy")
      .that_takes(10)
      .seconds()
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"timeGroup\" tests=\"1\" time=\"0.010\" "
      "timestamp=\"2013-07-04T22:28:00\">\n",
      output_file->line(3)
  );
}

TEST(JUnitOutput, withOneTestGroupAndMultipleTestCasesWithElapsedTime)
{
  test_case_runner->start()
      .with_group("twoTestsGroup")
      .with_test("firstTestName")
      .that_takes(10)
      .seconds()
      .with_test("secondTestName")
      .that_takes(50)
      .seconds()
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"twoTestsGroup\" tests=\"2\" time=\"0.060\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"firstTestName\" "
      "assertions=\"0\" time=\"0.010\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"secondTestName\" "
      "assertions=\"0\" time=\"0.050\" file=\"file\" line=\"1\">\n",
      output_file->line(6)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(7));
}

TEST(JUnitOutput, withOneTestGroupAndOneFailingTest)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("Test failed", "thisfile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"1\" skipped=\"0\" assertions=\"0\" "
      "name=\"testGroupWithFailingTest\" tests=\"1\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"testGroupWithFailingTest\" "
      "name=\"FailingTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL(
      "<failure message=\"thisfile:10: Test failed\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
  STRCMP_EQUAL("thisfile:10: Test failed\n", output_file->line(6));
  STRCMP_EQUAL("</failure>\n", output_file->line(7));
  STRCMP_EQUAL("</testcase>\n", output_file->line(8));
}

TEST(JUnitOutput, withTwoTestGroupAndOneFailingTest)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FirstTest")
      .with_test("FailingTestName")
      .that_fails("Test failed", "thisfile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"1\" skipped=\"0\" assertions=\"0\" "
      "name=\"testGroupWithFailingTest\" tests=\"2\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"testGroupWithFailingTest\" "
      "name=\"FailingTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(6)
  );
  STRCMP_EQUAL(
      "<failure message=\"thisfile:10: Test failed\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(7)
  );
}

TEST(JUnitOutput, testFailureWithLessThanAndGreaterThanInsideIt)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("Test <failed>", "thisfile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"thisfile:10: Test &lt;failed&gt;\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, testFailureWithQuotesInIt)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("Test \"failed\"", "thisfile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"thisfile:10: Test &quot;failed&quot;\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, testFailureWithNewlineInIt)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("Test \nfailed", "thisfile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"thisfile:10: Test &#10;failed\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, testFailureWithDifferentFileAndLine)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("Test failed", "importantFile", 999)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"importantFile:999: Test failed\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, testFailureWithAmpersandsAndLessThan)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("&object1 < &object2", "importantFile", 999)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"importantFile:999: &amp;object1 &lt; "
      "&amp;object2\" type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, testFailureWithAmpersands)
{
  test_case_runner->start()
      .with_group("testGroupWithFailingTest")
      .with_test("FailingTestName")
      .that_fails("&object1 != &object2", "importantFile", 999)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"importantFile:999: &amp;object1 != "
      "&amp;object2\" type=\"AssertionFailedError\">\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, aCoupleOfTestFailures)
{
  test_case_runner->start()
      .with_group("testGroup")
      .with_test("passingOne")
      .with_test("FailingTest")
      .that_fails("Failure", "file", 99)
      .with_test("passingTwo")
      .with_test("passingThree")
      .with_test("AnotherFailingTest")
      .that_fails("otherFailure", "anotherFile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"file:99: Failure\" type=\"AssertionFailedError\">\n",
      output_file->line(7)
  );
  STRCMP_EQUAL(
      "<failure message=\"anotherFile:10: otherFailure\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(16)
  );
}

TEST(JUnitOutput, testFailuresInSeparateGroups)
{
  test_case_runner->start()
      .with_group("testGroup")
      .with_test("passingOne")
      .with_test("FailingTest")
      .that_fails("Failure", "file", 99)
      .with_group("AnotherGroup")
      .with_test("AnotherFailingTest")
      .that_fails("otherFailure", "anotherFile", 10)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<failure message=\"file:99: Failure\" type=\"AssertionFailedError\">\n",
      output_file->line(7)
  );
  STRCMP_EQUAL(
      "<failure message=\"anotherFile:10: otherFailure\" "
      "type=\"AssertionFailedError\">\n",
      output_file->line(14)
  );
}

TEST(JUnitOutput, twoTestGroupsWriteToOneFile)
{
  test_case_runner->start()
      .with_group("firstTestGroup")
      .with_test("testName")
      .with_group("secondTestGroup")
      .with_test("testName")
      .end();

  CHECK_EQUAL(1, file_system.amount_of_files());
  CHECK(file_system.file("mutiny.xml") != nullptr);
}

TEST(JUnitOutput, testsuitesSummaryAggregatesAcrossGroups)
{
  test_case_runner->start()
      .with_group("groupA")
      .with_test("passing")
      .with_test("failing")
      .that_fails("boom", "a.cpp", 1)
      .with_test("skipped")
      .that_is_skipped("later")
      .with_group("groupB")
      .with_test("another")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuites tests=\"4\" failures=\"1\" errors=\"0\" skipped=\"1\" "
      "time=\"0.000\" timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(2)
  );
}

TEST(JUnitOutput, interleavedGroupsAreMergedIntoOneTestSuiteBlock)
{
  // With shuffled (-s) execution, tests from the same group are not
  // necessarily contiguous. groupA is visited, then groupB, then groupA
  // again. All of groupA's tests should end up in a single <testsuite>
  // block instead of being split across two blocks with the same name.
  test_case_runner->start()
      .with_group("groupA")
      .with_test("first")
      .with_group("groupB")
      .with_test("onlyB")
      .with_group("groupA")
      .with_test("second")
      .end();

  output_file = file_system.file("mutiny.xml");
  CHECK_EQUAL(size_t{ 13 }, output_file->amount_of_lines());

  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"groupA\" tests=\"2\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"groupA\" name=\"first\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"groupA\" name=\"second\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(6)
  );
  STRCMP_EQUAL("</testsuite>\n", output_file->line(8));

  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"groupB\" tests=\"1\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(9)
  );
}

TEST(JUnitOutput, packageNameWithReservedCharsEncodedInFileName)
{
  init("-pjunit=group/weird/name");
  test_case_runner->start().with_group("groupname").with_test("testname").end();
  CHECK(file_system.file_exists("group_weird_name.xml"));
}

TEST(JUnitOutput, TestCaseBlockWithAPackageName)
{
  init("-pjunit=packagename");
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("packagename.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"packagename.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
}

TEST(JUnitOutput, defaultPackageName_usedWhenNoPjunitSuffix)
{
  init("-pjunit", "mypackage");
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mypackage.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"mypackage.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, defaultPackageName_overriddenByExplicitSuffix)
{
  init("-pjunit=explicit", "mydefault");
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("explicit.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"explicit.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, emptyDefaultPackageName_producesNoPrefix)
{
  init("-pjunit");
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, TestCaseBlockForSkippedTest)
{
  init("-pjunit=packagename");
  test_case_runner->start()
      .with_group("groupname")
      .with_skipped_test("testname")
      .end();

  output_file = file_system.file("packagename.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"packagename.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("<skipped message=\"SKIPPED_TEST\" />\n", output_file->line(5));
  STRCMP_EQUAL("</testcase>\n", output_file->line(6));
}

TEST(JUnitOutput, TestCaseWithTestLocation)
{
  init("-pjunit=packagename");
  test_case_runner->start()
      .with_group("groupname")
      .with_test("testname")
      .in_file("MySource.c")
      .on_line(159)
      .end();

  output_file = file_system.file("packagename.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"packagename.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"MySource.c\" line=\"159\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, MultipleTestCaseWithTestLocations)
{
  test_case_runner->start()
      .with_group("twoTestsGroup")
      .with_test("firstTestName")
      .in_file("MyFirstSource.c")
      .on_line(846)
      .with_test("secondTestName")
      .in_file("MySecondSource.c")
      .on_line(513)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"firstTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"MyFirstSource.c\" "
      "line=\"846\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"secondTestName\" "
      "assertions=\"0\" time=\"0.000\" file=\"MySecondSource.c\" "
      "line=\"513\">\n",
      output_file->line(6)
  );
}

TEST(JUnitOutput, TestCaseBlockWithAssertions)
{
  init("-pjunit=packagename");
  test_case_runner->start()
      .with_group("groupname")
      .with_test("testname")
      .that_has_checks(24)
      .end();

  output_file = file_system.file("packagename.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"packagename.groupname\" name=\"testname\" "
      "assertions=\"24\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
}

TEST(JUnitOutput, MultipleTestCaseBlocksWithAssertions)
{
  test_case_runner->start()
      .with_group("twoTestsGroup")
      .with_test("firstTestName")
      .that_has_checks(456)
      .with_test("secondTestName")
      .that_has_checks(567)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"firstTestName\" "
      "assertions=\"456\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"twoTestsGroup\" name=\"secondTestName\" "
      "assertions=\"567\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(6)
  );
}

TEST(JUnitOutput, MultipleTestCasesInDifferentGroupsWithAssertions)
{
  test_case_runner->start()
      .with_group("groupOne")
      .with_test("testA")
      .that_has_checks(456)
      .end_group_and_clear_test()
      .with_group("groupTwo")
      .with_test("testB")
      .that_has_checks(678)
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"groupOne\" name=\"testA\" "
      "assertions=\"456\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL(
      "<testcase classname=\"groupTwo\" name=\"testB\" "
      "assertions=\"678\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(8)
  );
}

TEST(JUnitOutput, testWithNoOutputEmitsNoTestCaseSystemOut)
{
  test_case_runner->start().with_group("groupname").with_test("testname").end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
}

#if MUTINY_HAVE_EXCEPTIONS
TEST(JUnitOutput, unexpectedExceptionCountsAsErrorNotFailure)
{
  test_case_runner->start()
      .with_group("errorGroup")
      .with_test("errorTest")
      .that_errors()
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"1\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"errorGroup\" tests=\"1\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
}

TEST(JUnitOutput, unexpectedExceptionEmitsErrorElement)
{
  test_case_runner->start()
      .with_group("errorGroup")
      .with_test("errorTest")
      .that_errors()
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<error message=\"Unexpected exception of unknown type was thrown.\""
      " type=\"UnexpectedException\">\n",
      output_file->line(5)
  );
  STRCMP_EQUAL(
      "Unexpected exception of unknown type was thrown.\n", output_file->line(6)
  );
  STRCMP_EQUAL("</error>\n", output_file->line(7));
}

TEST(JUnitOutput, errorCountResetBetweenGroups)
{
  test_case_runner->start()
      .with_group("errorGroup")
      .with_test("errorTest")
      .that_errors()
      .with_group("cleanGroup")
      .with_test("passingTest")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"0\" failures=\"0\" skipped=\"0\" assertions=\"0\" "
      "name=\"cleanGroup\" tests=\"1\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(10)
  );
}

TEST(JUnitOutput, mixedErrorAndFailureCountedSeparately)
{
  test_case_runner->start()
      .with_group("mixedGroup")
      .with_test("failingTest")
      .that_fails("assertion failed", "somefile", 5)
      .with_test("errorTest")
      .that_errors()
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testsuite errors=\"1\" failures=\"1\" skipped=\"0\" assertions=\"0\" "
      "name=\"mixedGroup\" tests=\"2\" time=\"0.000\" "
      "timestamp=\"1978-10-03T00:00:00\">\n",
      output_file->line(3)
  );
}
#endif

TEST(JUnitOutput, testWithOnePropertyEmitsPropertiesBlock)
{
  test_case_runner->start()
      .with_group("propGroup")
      .with_test("propTest")
      .with_property("ticket_id", "12345")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"propGroup\" name=\"propTest\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("<properties>\n", output_file->line(5));
  STRCMP_EQUAL(
      "<property name=\"ticket_id\" value=\"12345\"/>\n", output_file->line(6)
  );
  STRCMP_EQUAL("</properties>\n", output_file->line(7));
  STRCMP_EQUAL("</testcase>\n", output_file->line(8));
}

TEST(JUnitOutput, testWithMultiplePropertiesEmitsAllInOrder)
{
  test_case_runner->start()
      .with_group("propGroup")
      .with_test("propTest")
      .with_property("ticket_id", "12345")
      .with_property("size", "10MB")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL("<properties>\n", output_file->line(5));
  STRCMP_EQUAL(
      "<property name=\"ticket_id\" value=\"12345\"/>\n", output_file->line(6)
  );
  STRCMP_EQUAL(
      "<property name=\"size\" value=\"10MB\"/>\n", output_file->line(7)
  );
  STRCMP_EQUAL("</properties>\n", output_file->line(8));
}

TEST(JUnitOutput, testWithNoPropertiesEmitsNoPropertiesBlock)
{
  test_case_runner->start()
      .with_group("noPropGroup")
      .with_test("noPropTest")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"noPropGroup\" name=\"noPropTest\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(5));
}

TEST(JUnitOutput, testPropertyNameAndValueAreXmlEncoded)
{
  test_case_runner->start()
      .with_group("encodeGroup")
      .with_test("encodeTest")
      .with_property("k&ey", "<val>")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<property name=\"k&amp;ey\" value=\"&lt;val&gt;\"/>\n",
      output_file->line(6)
  );
}

TEST(JUnitOutput, propertiesAccumulateCorrectlyAcrossGroups)
{
  test_case_runner->start()
      .with_group("groupA")
      .with_test("testA")
      .with_property("key", "val")
      .end_group_and_clear_test()
      .with_group("groupB")
      .with_test("testB")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL("<properties>\n", output_file->line(5));
  STRCMP_EQUAL(
      "<testcase classname=\"groupB\" name=\"testB\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(11)
  );
  STRCMP_EQUAL("</testcase>\n", output_file->line(12));
}

TEST(JUnitOutput, TestCaseBlockForSkippedTestWithMessage)
{
  init("-pjunit=packagename");
  test_case_runner->start()
      .with_group("groupname")
      .with_test("testname")
      .that_is_skipped("not ready yet")
      .end();

  output_file = file_system.file("packagename.xml");
  STRCMP_EQUAL(
      "<testcase classname=\"packagename.groupname\" name=\"testname\" "
      "assertions=\"0\" time=\"0.000\" file=\"file\" line=\"1\">\n",
      output_file->line(4)
  );
  STRCMP_EQUAL("<skipped message=\"not ready yet\" />\n", output_file->line(5));
  STRCMP_EQUAL("</testcase>\n", output_file->line(6));
}

TEST(JUnitOutput, TestCaseBlockForSkippedTestEscapesXmlInMessage)
{
  test_case_runner->start()
      .with_group("groupname")
      .with_test("testname")
      .that_is_skipped("skip <this> & \"that\"")
      .end();

  output_file = file_system.file("mutiny.xml");
  STRCMP_EQUAL(
      "<skipped message=\"skip &lt;this&gt; &amp; &quot;that&quot;\" />\n",
      output_file->line(5)
  );
}

TEST(JUnitOutput, parseArguments_pjunitWithoutName_succeeds)
{
  mu::tiny::test::JUnitOutputPlugin p;
  const char* argv[] = { "-pjunit" };
  CHECK(p.parse_arguments(1, argv));
}

TEST(JUnitOutput, parseArguments_pjunitWithInvalidSuffix_returnsFalse)
{
  mu::tiny::test::JUnitOutputPlugin p;
  const char* argv[] = { "-pjunitX" };
  CHECK(!p.parse_arguments(1, argv));
}
