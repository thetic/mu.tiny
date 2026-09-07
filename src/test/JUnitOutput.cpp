#include "mu/tiny/test/JUnitOutput.hpp"

#include "mu/tiny/test/Failure.hpp"
#include "mu/tiny/test/Output.hpp"
#include "mu/tiny/test/Result.hpp"
#include "mu/tiny/test/Shell.hpp"

#include "mu/tiny/time.hpp"

#include <inttypes.h>

namespace mu {
namespace tiny {
namespace test {

namespace {
constexpr uint_least64_t ms_per_s{ 1000 };

class TestProperty
{
public:
  String name;
  String value;
  TestProperty* next{ nullptr };
};

} // namespace

class JUnitTestCaseResultNode
{
public:
  JUnitTestCaseResultNode() = default;

  String name;
  uint_least64_t exec_time{ 0 };
  Failure* failure{ nullptr };
  bool failure_is_error{ false };
  bool skipped{ false };
  String skip_message;
  String file;
  int_least32_t line_number{ 0 };
  unsigned int check_count{ 0 };
  TestProperty* properties{ nullptr };
  TestProperty* properties_tail{ nullptr };
  JUnitTestCaseResultNode* next{ nullptr };
};

class JUnitTestGroupResult
{
public:
  JUnitTestGroupResult() = default;

  unsigned int test_count{ 0 };
  unsigned int failure_count{ 0 };
  unsigned int error_count{ 0 };
  unsigned int skip_count{ 0 };
  uint_least64_t group_exec_time{ 0 };
  String group;
  JUnitTestCaseResultNode* head{ nullptr };
  JUnitTestCaseResultNode* tail{ nullptr };
  JUnitTestGroupResult* next{ nullptr };
};

class JUnitTestOutputImpl
{
public:
  // Every group seen this run, keyed by name, in order of first
  // appearance. A group visited more than once (e.g. under `-s` shuffle,
  // which can interleave groups) keeps appending to the same bucket
  // instead of starting a new one, so its tests all land in a single
  // <testsuite> block.
  JUnitTestGroupResult* groups{ nullptr };
  // The bucket for the group currently running, or nullptr between
  // groups / when no test in the current group has started yet.
  JUnitTestGroupResult* current{ nullptr };
  unsigned int last_global_check_count{ 0 };
  String package;
  String start_timestamp;
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

JUnitTestGroupResult* JUnitOutput::find_or_create_group(const String& group)
{
  JUnitTestGroupResult* cur = impl_->groups;
  JUnitTestGroupResult* last = nullptr;
  while (cur != nullptr) {
    if (cur->group == group) {
      return cur;
    }
    last = cur;
    cur = cur->next;
  }

  auto* created = new JUnitTestGroupResult;
  created->group = group;
  if (last != nullptr) {
    last->next = created;
  } else {
    impl_->groups = created;
  }
  return created;
}

void JUnitOutput::reset_test_group_result()
{
  JUnitTestGroupResult* group = impl_->groups;
  while (group != nullptr) {
    JUnitTestCaseResultNode* cur = group->head;
    while (cur != nullptr) {
      JUnitTestCaseResultNode* tmp = cur->next;
      delete cur->failure;
      TestProperty* prop = cur->properties;
      while (prop != nullptr) {
        TestProperty* prop_tmp = prop->next;
        delete prop;
        prop = prop_tmp;
      }
      delete cur;
      cur = tmp;
    }
    JUnitTestGroupResult* next = group->next;
    delete group;
    group = next;
  }
  impl_->groups = nullptr;
  impl_->current = nullptr;
}

void JUnitOutput::print_tests_started()
{
  reset_test_group_result();
  impl_->last_global_check_count = 0;
  impl_->start_timestamp = get_time_string();
}

void JUnitOutput::print_current_group_started(const Shell& /*test*/)
{
  impl_->current = nullptr;
}

void JUnitOutput::print_current_test_ended(const Result& result)
{
  const unsigned int global_check_count = result.get_check_count();
  impl_->current->tail->exec_time =
      result.get_current_test_total_execution_time();
  impl_->current->tail->check_count =
      global_check_count - impl_->last_global_check_count;
  impl_->last_global_check_count = global_check_count;
}

void JUnitOutput::print_tests_ended(const Result& /*result*/)
{
  unsigned int total_test_count = 0;
  unsigned int total_failure_count = 0;
  unsigned int total_error_count = 0;
  unsigned int total_skip_count = 0;
  uint_least64_t total_exec_time = 0;
  for (JUnitTestGroupResult* g = impl_->groups; g != nullptr; g = g->next) {
    total_test_count += g->test_count;
    total_failure_count += g->failure_count;
    total_error_count += g->error_count;
    total_skip_count += g->skip_count;
    total_exec_time += g->group_exec_time;
  }

  Output::File file = fopen_(create_file_name().c_str(), "w");
  String header = string_from_format(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
      "<testsuites tests=\"%d\" failures=\"%d\" errors=\"%d\" "
      "skipped=\"%d\" time=\"%d.%03d\" timestamp=\"%s\">\n",
      static_cast<int>(total_test_count),
      static_cast<int>(total_failure_count),
      static_cast<int>(total_error_count),
      static_cast<int>(total_skip_count),
      static_cast<int>(total_exec_time / ms_per_s),
      static_cast<int>(total_exec_time % ms_per_s),
      impl_->start_timestamp.c_str()
  );
  fputs_(header.c_str(), file);

  for (JUnitTestGroupResult* g = impl_->groups; g != nullptr; g = g->next) {
    write_test_group_to_file(file, g);
  }

  fputs_("</testsuites>\n", file);
  fclose_(file);
}

void JUnitOutput::print_current_group_ended(const Result& result)
{
  if (impl_->current == nullptr) {
    return;
  }
  impl_->current->group_exec_time +=
      result.get_current_group_total_execution_time();
  impl_->current = nullptr;
}

void JUnitOutput::print_current_test_started(const Shell& test)
{
  impl_->current = find_or_create_group(test.get_group());
  impl_->current->test_count++;

  if (impl_->current->tail == nullptr) {
    impl_->current->head = impl_->current->tail = new JUnitTestCaseResultNode;
  } else {
    impl_->current->tail->next = new JUnitTestCaseResultNode;
    impl_->current->tail = impl_->current->tail->next;
  }
  impl_->current->tail->name = test.get_name();
  impl_->current->tail->file = test.get_file();
  impl_->current->tail->line_number = test.get_line_number();
  if (!test.will_run()) {
    impl_->current->tail->skipped = true;
    impl_->current->tail->skip_message = test.get_macro_name();
    impl_->current->skip_count++;
  }
}

String JUnitOutput::create_file_name()
{
  if (!impl_->package.empty()) {
    return encode_file_name(impl_->package) + ".xml";
  }
  return "mutiny.xml";
}

String JUnitOutput::encode_file_name(const String& file_name)
{
  // special character list based on: https://en.wikipedia.org/wiki/Filename
  static const char* const forbidden_characters = "/\\?%*:|\"<>";

  String result = file_name;
  for (const char* sym = forbidden_characters; *sym != 0; ++sym) {
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

void JUnitOutput::write_test_suite_summary(
    Output::File file,
    JUnitTestGroupResult* group
)
{
  unsigned int total_assertions = 0;
  for (JUnitTestCaseResultNode* n = group->head; n != nullptr; n = n->next) {
    total_assertions += n->check_count;
  }

  String buf = string_from_format(
      "<testsuite errors=\"%d\" failures=\"%d\" skipped=\"%d\" "
      "assertions=\"%d\" name=\"%s\" tests=\"%d\" "
      "time=\"%d.%03d\" timestamp=\"%s\">\n",
      static_cast<int>(group->error_count),
      static_cast<int>(group->failure_count),
      static_cast<int>(group->skip_count),
      static_cast<int>(total_assertions),
      group->group.c_str(),
      static_cast<int>(group->test_count),
      static_cast<int>(group->group_exec_time / ms_per_s),
      static_cast<int>(group->group_exec_time % ms_per_s),
      get_time_string()
  );
  write_to_file(file, buf.c_str());
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

void JUnitOutput::write_test_cases(
    Output::File file,
    JUnitTestGroupResult* group
)
{
  JUnitTestCaseResultNode* cur = group->head;

  while (cur != nullptr) {
    String buf = string_from_format(
        "<testcase classname=\"%s%s%s\" name=\"%s\" assertions=\"%d\" "
        "time=\"%d.%03d\" file=\"%s\" line=\"%" PRIdLEAST32 "\">\n",
        impl_->package.c_str(),
        impl_->package.empty() ? "" : ".",
        group->group.c_str(),
        cur->name.c_str(),
        static_cast<int>(cur->check_count),
        static_cast<int>(cur->exec_time / ms_per_s),
        static_cast<int>(cur->exec_time % ms_per_s),
        cur->file.c_str(),
        cur->line_number
    );
    write_to_file(file, buf.c_str());

    if (cur->properties != nullptr) {
      write_to_file(file, "<properties>\n");
      for (TestProperty* prop = cur->properties; prop != nullptr;
           prop = prop->next) {
        String prop_buf = string_from_format(
            "<property name=\"%s\" value=\"%s\"/>\n",
            encode_xml_text(prop->name).c_str(),
            encode_xml_text(prop->value).c_str()
        );
        write_to_file(file, prop_buf.c_str());
      }
      write_to_file(file, "</properties>\n");
    }

    if (cur->failure != nullptr) {
      if (cur->failure_is_error) {
        write_error(file, cur);
      } else {
        write_failure(file, cur);
      }
    } else if (cur->skipped) {
      if (cur->skip_message.empty()) {
        write_to_file(file, "<skipped />\n");
      } else {
        write_to_file(
            file,
            string_from_format(
                "<skipped message=\"%s\" />\n",
                encode_xml_text(cur->skip_message).c_str()
            )
                .c_str()
        );
      }
    }

    write_to_file(file, "</testcase>\n");
    cur = cur->next;
  }
}

void JUnitOutput::write_failure(
    Output::File file,
    JUnitTestCaseResultNode* node
)
{
  String failure_file = encode_xml_text(node->failure->get_file_name());
  String msg = encode_xml_text(node->failure->get_message());
  String buf = string_from_format(
      "<failure message=\"%s:%" PRIdLEAST32
      ": %s\" type=\"AssertionFailedError\">\n"
      "%s:%" PRIdLEAST32 ": %s\n",
      failure_file.c_str(),
      node->failure->get_failure_line_number(),
      msg.c_str(),
      failure_file.c_str(),
      node->failure->get_failure_line_number(),
      msg.c_str()
  );
  write_to_file(file, buf.c_str());
  write_to_file(file, "</failure>\n");
}

void JUnitOutput::write_error(Output::File file, JUnitTestCaseResultNode* node)
{
  String msg = encode_xml_text(node->failure->get_message());
  String buf = string_from_format(
      "<error message=\"%s\" type=\"UnexpectedException\">\n"
      "%s\n",
      msg.c_str(),
      msg.c_str()
  );
  write_to_file(file, buf.c_str());
  write_to_file(file, "</error>\n");
}

void JUnitOutput::write_file_ending(Output::File file)
{
  write_to_file(file, "</testsuite>\n");
}

void JUnitOutput::write_test_group_to_file(
    Output::File file,
    JUnitTestGroupResult* group
)
{
  write_test_suite_summary(file, group);
  write_test_cases(file, group);
  write_file_ending(file);
}

void JUnitOutput::print_buffer(const char* /*buffer*/) {}

void JUnitOutput::print_test_property(const char* name, const char* value)
{
  if (impl_->current->tail == nullptr) {
    return;
  }
  auto* prop = new TestProperty;
  prop->name = name;
  prop->value = value;
  if (impl_->current->tail->properties == nullptr) {
    impl_->current->tail->properties = prop;
    impl_->current->tail->properties_tail = prop;
  } else {
    impl_->current->tail->properties_tail->next = prop;
    impl_->current->tail->properties_tail = prop;
  }
}

void JUnitOutput::print_skipped(const char* message)
{
  if (impl_->current->tail == nullptr) {
    return;
  }
  impl_->current->tail->skipped = true;
  impl_->current->tail->skip_message = message;
  impl_->current->skip_count++;
}

void JUnitOutput::print_failure(const Failure& failure)
{
  if (impl_->current->tail->failure == nullptr) {
    if (failure.is_error()) {
      impl_->current->error_count++;
      impl_->current->tail->failure_is_error = true;
    } else {
      impl_->current->failure_count++;
    }
    impl_->current->tail->failure = new Failure(failure);
  }
}

void JUnitOutput::write_to_file(Output::File file, const String& buffer)
{
  fputs_(buffer.c_str(), file);
}

} // namespace test
} // namespace tiny
} // namespace mu
