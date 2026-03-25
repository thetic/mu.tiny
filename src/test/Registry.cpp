#include "mutiny/test/Registry.hpp"

#include "mutiny/test/NullPlugin.hpp"
#include "mutiny/test/Result.hpp"
#include "mutiny/test/Shell.hpp"
#include "mutiny/test/ShellPointerArray.hpp"

namespace mu {
namespace tiny {
namespace test {

Registry::Registry()
  : first_plugin_(NullPlugin::instance())

{
}

void Registry::add_test(Shell* test)
{
  tests_ = test->add_test(tests_);
}

void Registry::run_all_tests(Result& result)
{
  bool group_start = true;

  result.tests_started();
  for (Shell* test = tests_; test != nullptr; test = test->get_next()) {
    if (run_ignored_)
      test->set_run_ignored();

    if (group_start) {
      result.current_group_started(test);
      group_start = false;
    }

    result.count_test();
    if (test_should_run(test, result)) {
      result.current_test_started(test);
      test->run_one_test(first_plugin_, result);
      result.current_test_ended(test);
    }

    if (end_of_group(test)) {
      group_start = true;
      result.current_group_ended(test);
    }
  }
  result.tests_ended();
  current_repetition_++;
}

void Registry::list_test_group_names(Result& result)
{
  String group_list;

  for (Shell* test = tests_; test != nullptr; test = test->get_next()) {
    String gname;
    gname += MUTINY_STR("#");
    gname += test->get_group();
    gname += MUTINY_STR("#");

    if (!string_contains(group_list, gname)) {
      group_list += gname;
      group_list += MUTINY_STR(" ");
    }
  }

  string_replace(group_list, "#", "");

  if (string_ends_with(group_list, " "))
    group_list = group_list.substr(0, group_list.size() - 1);
  result.print(group_list.c_str());
}

void Registry::list_test_group_and_case_names(Result& result)
{
  String group_and_name_list;

  for (Shell* test = tests_; test != nullptr; test = test->get_next()) {
    if (test_should_run(test, result)) {
      String group_and_name;
      group_and_name += MUTINY_STR("#");
      group_and_name += test->get_group();
      group_and_name += MUTINY_STR(".");
      group_and_name += test->get_name();
      group_and_name += MUTINY_STR("#");

      if (!string_contains(group_and_name_list, group_and_name)) {
        group_and_name_list += group_and_name;
        group_and_name_list += MUTINY_STR(" ");
      }
    }
  }

  string_replace(group_and_name_list, "#", "");

  if (string_ends_with(group_and_name_list, " "))
    group_and_name_list =
        group_and_name_list.substr(0, group_and_name_list.size() - 1);
  result.print(group_and_name_list.c_str());
}

void Registry::list_test_locations(Result& result)
{
  String test_locations;

  for (Shell* test = tests_; test != nullptr; test = test->get_next()) {
    String test_location;
    test_location += test->get_group();
    test_location += MUTINY_STR(".");
    test_location += test->get_name();
    test_location += MUTINY_STR(".");
    test_location += test->get_file();
    test_location += MUTINY_STR(".");
    test_location +=
        string_from_format("%d\n", static_cast<int>(test->get_line_number()));

    test_locations += test_location;
  }

  result.print(test_locations.c_str());
}

void Registry::list_ordered_test_locations(Result& result)
{
  String test_locations;

  for (Shell* test = tests_; test != nullptr; test = test->get_next()) {
    if (!test->is_ordered())
      continue;
    String test_location;
    test_location += test->get_group();
    test_location += MUTINY_STR(".");
    test_location += test->get_name();
    test_location += MUTINY_STR(".");
    test_location += test->get_file();
    test_location += MUTINY_STR(".");
    test_location +=
        string_from_format("%d\n", static_cast<int>(test->get_line_number()));

    test_locations += test_location;
  }

  result.print(test_locations.c_str());
}

bool Registry::end_of_group(Shell* test)
{
  return (
      !test || !test->get_next() ||
      test->get_group() != test->get_next()->get_group()
  );
}

size_t Registry::count_tests()
{
  return tests_ ? tests_->count_tests() : 0;
}

Registry* Registry::current_registry_ = nullptr;

Registry* Registry::get_current_registry()
{
  static Registry registry;
  return (current_registry_ == nullptr) ? &registry : current_registry_;
}

void Registry::set_current_registry(Registry* registry)
{
  current_registry_ = registry;
}

void Registry::un_do_last_add_test()
{
  tests_ = tests_ ? tests_->get_next() : nullptr;
}

void Registry::set_name_filters(const Filter* filters)
{
  name_filters_ = filters;
}

void Registry::set_group_filters(const Filter* filters)
{
  group_filters_ = filters;
}

void Registry::set_run_ignored()
{
  run_ignored_ = true;
}

int Registry::get_current_repetition()
{
  return current_repetition_;
}

bool Registry::test_should_run(Shell* test, Result& result)
{
  if (test->should_run(group_filters_, name_filters_))
    return true;
  else {
    result.count_filtered_out();
    return false;
  }
}

void Registry::reset_plugins()
{
  first_plugin_ = NullPlugin::instance();
}

void Registry::install_plugin(Plugin* plugin)
{
  first_plugin_ = plugin->add_plugin(first_plugin_);
}

Plugin* Registry::get_first_plugin()
{
  return first_plugin_;
}

Plugin* Registry::get_plugin_by_name(const String& name)
{
  return first_plugin_->get_plugin_by_name(name);
}

void Registry::remove_plugin_by_name(const String& name)
{
  first_plugin_->remove_plugin_by_name(name);
  if (first_plugin_->get_name() == name)
    first_plugin_ = first_plugin_->get_next();
  first_plugin_->remove_plugin_by_name(name);
}

int Registry::count_plugins()
{
  int count = 0;
  for (Plugin* plugin = first_plugin_; plugin != NullPlugin::instance();
       plugin = plugin->get_next())
    count++;
  return count;
}

Shell* Registry::get_first_test()
{
  return tests_;
}

void Registry::shuffle_tests(size_t seed)
{
  ShellPointerArray array(get_first_test());
  array.shuffle(seed);
  tests_ = array.get_first_test();
}

void Registry::reverse_tests()
{
  ShellPointerArray array(get_first_test());
  array.reverse();
  tests_ = array.get_first_test();
}

Shell* Registry::get_test_with_next(Shell* test)
{
  Shell* current = tests_;
  while (current && current->get_next() != test)
    current = current->get_next();
  return current;
}

Shell* Registry::find_test_with_name(const String& name)
{
  Shell* current = tests_;
  while (current) {
    if (current->get_name() == name)
      return current;
    current = current->get_next();
  }
  return nullptr;
}

Shell* Registry::find_test_with_group(const String& group)
{
  Shell* current = tests_;
  while (current) {
    if (current->get_group() == group)
      return current;
    current = current->get_next();
  }
  return nullptr;
}

} // namespace test
} // namespace tiny
} // namespace mu
