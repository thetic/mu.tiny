#include "mutiny/mock/ActualCallTrace.hpp"

namespace mu {
namespace tiny {
namespace mock {
using namespace mu::tiny::test;

ActualCallTrace* ActualCallTrace::instance_ = nullptr;

ActualCall& ActualCallTrace::with_name(const String& name)
{
  trace_buffer_ += MUTINY_STR("\nFunction name:");
  trace_buffer_ += name;
  return *this;
}

ActualCall& ActualCallTrace::with_call_order(unsigned int call_order)
{
  trace_buffer_ += MUTINY_STR(" withCallOrder:");
  trace_buffer_ += mu::tiny::test::string_from(call_order);
  return *this;
}

void ActualCallTrace::add_parameter_name(const String& name)
{
  trace_buffer_ += MUTINY_STR(" ");
  trace_buffer_ += name;
  trace_buffer_ += MUTINY_STR(":");
}

ActualCall& ActualCallTrace::with_bool_parameter(const String& name, bool value)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_unsigned_int_parameter(
    const String& name,
    unsigned int value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_int_parameter(const String& name, int value)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_unsigned_long_int_parameter(
    const String& name,
    unsigned long int value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_long_int_parameter(
    const String& name,
    long int value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_unsigned_long_long_int_parameter(
    const String& name,
    unsigned long long value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_long_long_int_parameter(
    const String& name,
    long long value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value) + MUTINY_STR(" ") +
                   brackets_formatted_hex_string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_double_parameter(
    const String& name,
    double value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_string_parameter(
    const String& name,
    const char* value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_pointer_parameter(
    const String& name,
    void* value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_const_pointer_parameter(
    const String& name,
    const void* value
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_function_pointer_parameter(
    const String& name,
    void (*value)()
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_memory_buffer_parameter(
    const String& name,
    const unsigned char* value,
    size_t size
)
{
  add_parameter_name(name);
  trace_buffer_ += string_from_binary_with_size_or_null(value, size);
  return *this;
}

ActualCall& ActualCallTrace::with_parameter_of_type(
    const String& type_name,
    const String& name,
    const void* value
)
{
  trace_buffer_ += MUTINY_STR(" ");
  trace_buffer_ += type_name;
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_memory_buffer_parameter(
    const char* name,
    const unsigned char* value,
    size_t size
)
{
  return with_memory_buffer_parameter(String(name), value, size);
}

ActualCall& ActualCallTrace::with_parameter_of_type(
    const char* type_name,
    const char* name,
    const void* value
)
{
  return with_parameter_of_type(String(type_name), String(name), value);
}

ActualCall& ActualCallTrace::with_output_parameter(
    const String& name,
    void* output
)
{
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(output);
  return *this;
}

ActualCall& ActualCallTrace::with_output_parameter_of_type(
    const String& type_name,
    const String& name,
    void* output
)
{
  trace_buffer_ += MUTINY_STR(" ");
  trace_buffer_ += type_name;
  add_parameter_name(name);
  trace_buffer_ += mu::tiny::test::string_from(output);
  return *this;
}

bool ActualCallTrace::has_return_value()
{
  return false;
}

NamedValue ActualCallTrace::return_value()
{
  return NamedValue("");
}

long int ActualCallTrace::return_long_int_value()
{
  return 0;
}

unsigned long int ActualCallTrace::return_unsigned_long_int_value()
{
  return 0;
}

unsigned long int ActualCallTrace::return_unsigned_long_int_value_or_default(
    unsigned long
)
{
  return 0;
}

long int ActualCallTrace::return_long_int_value_or_default(long int)
{
  return 0;
}

long long ActualCallTrace::return_long_long_int_value()
{
  return 0;
}

unsigned long long ActualCallTrace::return_unsigned_long_long_int_value()
{
  return 0;
}

unsigned long long ActualCallTrace::
    return_unsigned_long_long_int_value_or_default(unsigned long long)
{
  return 0;
}

long long ActualCallTrace::return_long_long_int_value_or_default(long long)
{
  return 0;
}

bool ActualCallTrace::return_bool_value()
{
  return false;
}

bool ActualCallTrace::return_bool_value_or_default(bool)
{
  return false;
}

int ActualCallTrace::return_int_value()
{
  return 0;
}

double ActualCallTrace::return_double_value()
{
  return 0.0;
}

double ActualCallTrace::return_double_value_or_default(double)
{
  return return_double_value();
}

unsigned int ActualCallTrace::return_unsigned_int_value()
{
  return 0;
}

void* ActualCallTrace::return_pointer_value()
{
  return nullptr;
}

const void* ActualCallTrace::return_const_pointer_value()
{
  return nullptr;
}

ActualCall::FunctionPointerReturnValue ActualCallTrace::
    return_function_pointer_value()
{
  return nullptr;
}

const void* ActualCallTrace::return_const_pointer_value_or_default(const void*)
{
  return return_const_pointer_value();
}

void* ActualCallTrace::return_pointer_value_or_default(void*)
{
  return return_pointer_value();
}

ActualCall::FunctionPointerReturnValue ActualCallTrace::
    return_function_pointer_value_or_default(void (*)())
{
  return return_function_pointer_value();
}

const char* ActualCallTrace::return_string_value()
{
  return "";
}

const char* ActualCallTrace::return_string_value_or_default(const char*)
{
  return return_string_value();
}

int ActualCallTrace::return_int_value_or_default(int)
{
  return 0;
}

unsigned int ActualCallTrace::return_unsigned_int_value_or_default(unsigned int)
{
  return return_unsigned_int_value();
}

ActualCall& ActualCallTrace::on_object(const void* object_ptr)
{
  trace_buffer_ += MUTINY_STR(" onObject:");
  trace_buffer_ += mu::tiny::test::string_from(object_ptr);
  return *this;
}

void ActualCallTrace::clear()
{
  trace_buffer_ = "";
}

const char* ActualCallTrace::get_trace_output()
{
  return trace_buffer_.c_str();
}

ActualCallTrace& ActualCallTrace::instance()
{
  if (instance_ == nullptr)
    instance_ = new ActualCallTrace;
  return *instance_;
}

void ActualCallTrace::clear_instance()
{
  delete instance_;
  instance_ = nullptr;
}

} // namespace mock
} // namespace tiny
} // namespace mu
