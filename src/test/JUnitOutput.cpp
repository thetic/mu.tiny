#include "mutiny/test/JUnitOutput.hpp"

#include "mutiny/test/Failure.hpp"
#include "mutiny/test/Output.hpp"
#include "mutiny/test/Result.hpp"
#include "mutiny/test/Shell.hpp"
#include "mutiny/test/time.hpp"

namespace mu {
namespace tiny {
namespace test {

namespace {

struct TestProperty
{
  String name;
  String value;
  TestProperty* next{ nullptr };
};

} // namespace

struct JUnitTestCaseResultNode
{
  JUnitTestCaseResultNode() = default;

  String name;
  size_t exec_time{ 0 };
  Failure* failure{ nullptr };
  bool failure_is_error{ false };
  bool ignored{ false };
  String file;
  size_t line_number{ 0 };
  size_t check_count{ 0 };
  TestProperty* properties{ nullptr };
  TestProperty* properties_tail{ nullptr };
  JUnitTestCaseResultNode* next{ nullptr };
};

struct JUnitTestGroupResult
{
  JUnitTestGroupResult() = default;

  size_t test_count{ 0 };
  size_t failure_count{ 0 };
  size_t error_count{ 0 };
  size_t total_check_count{ 0 };
  size_t start_time{ 0 };
  size_t group_exec_time{ 0 };
  String group;
  JUnitTestCaseResultNode* head{ nullptr };
  JUnitTestCaseResultNode* tail{ nullptr };
};

struct JUnitTestOutputImpl
{
  JUnitTestGroupResult results;
  Output::File file;
  String package;
  String std_output;
};

JUnitOutput::JUnitOutput()
  : impl_(new JUnitTestOutputImpl)
{
}

JUnitOutput::~JUnitOutput()
{
  reset_test_group_result();
  delete impl_;
}

void JUnitOutput::reset_test_group_result()
{
  impl_->results.test_count = 0;
  impl_->results.failure_count = 0;
  impl_->results.error_count = 0;
  impl_->results.group = "";
  JUnitTestCaseResultNode* cur = impl_->results.head;
  while (cur) {
    JUnitTestCaseResultNode* tmp = cur->next;
    delete cur->failure;
    TestProperty* prop = cur->properties;
    while (prop) {
      TestProperty* prop_tmp = prop->next;
      delete prop;
      prop = prop_tmp;
    }
    delete cur;
    cur = tmp;
  }
  impl_->results.head = nullptr;
  impl_->results.tail = nullptr;
}

void JUnitOutput::print_tests_started() {}

void JUnitOutput::print_current_group_started(const Shell& /*test*/) {}

void JUnitOutput::print_current_test_ended(const Result& result)
{
  impl_->results.tail->exec_time =
      result.get_current_test_total_execution_time();
  impl_->results.tail->check_count = result.get_check_count();
}

void JUnitOutput::print_tests_ended(const Result& /*result*/) {}

void JUnitOutput::print_current_group_ended(const Result& result)
{
  impl_->results.group_exec_time =
      result.get_current_group_total_execution_time();
  write_test_group_to_file();
  reset_test_group_result();
}

void JUnitOutput::print_current_test_started(const Shell& test)
{
  impl_->results.test_count++;
  impl_->results.group = test.get_group();
  impl_->results.start_time = static_cast<size_t>(get_time_in_millis());

  if (impl_->results.tail == nullptr) {
    impl_->results.head = impl_->results.tail = new JUnitTestCaseResultNode;
  } else {
    impl_->results.tail->next = new JUnitTestCaseResultNode;
    impl_->results.tail = impl_->results.tail->next;
  }
  impl_->results.tail->name = test.get_name();
  impl_->results.tail->file = test.get_file();
  impl_->results.tail->line_number = test.get_line_number();
  if (!test.will_run()) {
    impl_->results.tail->ignored = true;
  }
}

String JUnitOutput::create_file_name(const String& group)
{
  String file_name = "mutiny_";
  if (!impl_->package.empty()) {
    file_name += impl_->package;
    file_name += MUTINY_STR("_");
  }
  file_name += group;
  return encode_file_name(file_name) + MUTINY_STR(".xml");
}

String JUnitOutput::encode_file_name(const String& file_name)
{
  // special character list based on: https://en.wikipedia.org/wiki/Filename
  static const char* const forbidden_characters = "/\\?%*:|\"<>";

  String result = file_name;
  for (const char* sym = forbidden_characters; *sym; ++sym) {
    string_replace(result, *sym, '_');
  }
  return result;
}

void JUnitOutput::set_package_name(const String& package)
{
  if (impl_ != nullptr) {
    impl_->package = package;
  }
}

void JUnitOutput::write_xml_header()
{
  write_to_file("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
}

void JUnitOutput::write_test_suite_summary()
{
  String buf = string_from_format(
      "<testsuite errors=\"%d\" failures=\"%d\" "
      "name=\"%s\" tests=\"%d\" time=\"%d.%03d\" timestamp=\"%s\">\n",
      static_cast<int>(impl_->results.error_count),
      static_cast<int>(impl_->results.failure_count),
      impl_->results.group.c_str(),
      static_cast<int>(impl_->results.test_count),
      static_cast<int>(impl_->results.group_exec_time / 1000),
      static_cast<int>(impl_->results.group_exec_time % 1000),
      get_time_string()
  );
  write_to_file(buf.c_str());
}

void JUnitOutput::write_properties()
{
  write_to_file("<properties>\n");
  write_to_file("</properties>\n");
}

String JUnitOutput::encode_xml_text(const String& textbody)
{
  String buf = textbody.c_str();
  string_replace(buf, "&", "&amp;");
  string_replace(buf, "\"", "&quot;");
  string_replace(buf, "<", "&lt;");
  string_replace(buf, ">", "&gt;");
  string_replace(buf, "\r", "&#13;");
  string_replace(buf, "\n", "&#10;");
  return buf;
}

void JUnitOutput::write_test_cases()
{
  JUnitTestCaseResultNode* cur = impl_->results.head;

  while (cur) {
    String buf = string_from_format(
        "<testcase classname=\"%s%s%s\" name=\"%s\" assertions=\"%d\" "
        "time=\"%d.%03d\" file=\"%s\" line=\"%d\">\n",
        impl_->package.c_str(),
        impl_->package.empty() ? "" : ".",
        impl_->results.group.c_str(),
        cur->name.c_str(),
        static_cast<int>(cur->check_count - impl_->results.total_check_count),
        static_cast<int>(cur->exec_time / 1000),
        static_cast<int>(cur->exec_time % 1000),
        cur->file.c_str(),
        static_cast<int>(cur->line_number)
    );
    write_to_file(buf.c_str());

    impl_->results.total_check_count = cur->check_count;

    if (cur->properties) {
      write_to_file("<properties>\n");
      for (TestProperty* prop = cur->properties; prop; prop = prop->next) {
        String prop_buf = string_from_format(
            "<property name=\"%s\" value=\"%s\"/>\n",
            encode_xml_text(prop->name).c_str(),
            encode_xml_text(prop->value).c_str()
        );
        write_to_file(prop_buf.c_str());
      }
      write_to_file("</properties>\n");
    }

    if (cur->failure) {
      if (cur->failure_is_error)
        write_error(cur);
      else
        write_failure(cur);
    } else if (cur->ignored) {
      write_to_file("<skipped />\n");
    }
    write_to_file("</testcase>\n");
    cur = cur->next;
  }
}

void JUnitOutput::write_failure(JUnitTestCaseResultNode* node)
{
  String msg = encode_xml_text(node->failure->get_message());
  String buf = string_from_format(
      "<failure message=\"%s:%d: %s\" type=\"AssertionFailedError\">\n"
      "%s:%d: %s\n",
      node->failure->get_file_name().c_str(),
      static_cast<int>(node->failure->get_failure_line_number()),
      msg.c_str(),
      node->failure->get_file_name().c_str(),
      static_cast<int>(node->failure->get_failure_line_number()),
      msg.c_str()
  );
  write_to_file(buf.c_str());
  write_to_file("</failure>\n");
}

void JUnitOutput::write_error(JUnitTestCaseResultNode* node)
{
  String msg = encode_xml_text(node->failure->get_message());
  String buf = string_from_format(
      "<error message=\"%s\" type=\"UnexpectedException\">\n"
      "%s\n",
      msg.c_str(),
      msg.c_str()
  );
  write_to_file(buf.c_str());
  write_to_file("</error>\n");
}

void JUnitOutput::write_file_ending()
{
  write_to_file("<system-out>");
  write_to_file(encode_xml_text(impl_->std_output));
  write_to_file("</system-out>\n");
  write_to_file("<system-err></system-err>\n");
  write_to_file("</testsuite>\n");
}

void JUnitOutput::write_test_group_to_file()
{
  open_file_for_write(create_file_name(impl_->results.group));
  write_xml_header();
  write_test_suite_summary();
  write_properties();
  write_test_cases();
  write_file_ending();
  close_file();
}

void JUnitOutput::print_buffer(const char*) {}

void JUnitOutput::print(const char* output)
{
  impl_->std_output += output;
}

void JUnitOutput::print(long) {}

void JUnitOutput::print(size_t) {}

void JUnitOutput::print_test_property(const char* name, const char* value)
{
  if (impl_->results.tail == nullptr)
    return;
  auto* prop = new TestProperty;
  prop->name = name;
  prop->value = value;
  if (impl_->results.tail->properties == nullptr) {
    impl_->results.tail->properties = prop;
    impl_->results.tail->properties_tail = prop;
  } else {
    impl_->results.tail->properties_tail->next = prop;
    impl_->results.tail->properties_tail = prop;
  }
}

void JUnitOutput::print_failure(const Failure& failure)
{
  if (impl_->results.tail->failure == nullptr) {
    if (failure.is_error()) {
      impl_->results.error_count++;
      impl_->results.tail->failure_is_error = true;
    } else {
      impl_->results.failure_count++;
    }
    impl_->results.tail->failure = new Failure(failure);
  }
}

void JUnitOutput::open_file_for_write(const String& file_name)
{
  impl_->file = fopen_(file_name.c_str(), "w");
}

void JUnitOutput::write_to_file(const String& buffer)
{
  fputs_(buffer.c_str(), impl_->file);
}

void JUnitOutput::close_file()
{
  fclose_(impl_->file);
}

} // namespace test
} // namespace tiny
} // namespace mu
