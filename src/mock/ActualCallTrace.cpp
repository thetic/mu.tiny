#include "mu/tiny/mock/ActualCallTrace.hpp"

namespace mu {
namespace tiny {
namespace mock {

ActualCallTrace* ActualCallTrace::instance_ = nullptr;

ActualCall& ActualCallTrace::with_name(StringView name)
{
  trace_buffer_ += "\nFunction name:";
  trace_buffer_ += name.c_str();
  return *this;
}

ActualCall& ActualCallTrace::with_call_order(unsigned int call_order)
{
  trace_buffer_ += " withCallOrder:";
  trace_buffer_ += string_from(call_order);
  return *this;
}

void ActualCallTrace::add_parameter_name(StringView name)
{
  trace_buffer_ += " ";
  trace_buffer_ += name.c_str();
  trace_buffer_ += ":";
}

ActualCall& ActualCallTrace::with_typed_parameter(NamedValue parameter)
{
  add_parameter_name(parameter.get_name());
  trace_buffer_ += parameter.to_string();
  return *this;
}

ActualCall& ActualCallTrace::with_parameter_of_type(
    StringView type_name,
    StringView name,
    const void* value
)
{
  trace_buffer_ += " ";
  trace_buffer_ += type_name.c_str();
  add_parameter_name(name);
  trace_buffer_ += string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_output_parameter(
    StringView name,
    void* output
)
{
  add_parameter_name(name);
  trace_buffer_ += string_from(output);
  return *this;
}

ActualCall& ActualCallTrace::with_output_parameter_of_type(
    StringView type_name,
    StringView name,
    void* output
)
{
  trace_buffer_ += " ";
  trace_buffer_ += type_name.c_str();
  add_parameter_name(name);
  trace_buffer_ += string_from(output);
  return *this;
}

ActualCall& ActualCallTrace::with_captured_parameter(
    StringView name,
    const void* value
)
{
  add_parameter_name(name);
  trace_buffer_ += string_from(value);
  return *this;
}

ActualCall& ActualCallTrace::with_captured_parameter_of_type(
    StringView type_name,
    StringView name,
    const void* value
)
{
  trace_buffer_ += " ";
  trace_buffer_ += type_name.c_str();
  add_parameter_name(name);
  trace_buffer_ += string_from(value);
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

ActualCall& ActualCallTrace::on_object(const void* object_ptr)
{
  trace_buffer_ += " onObject:";
  trace_buffer_ += string_from(object_ptr);
  return *this;
}

const char* ActualCallTrace::get_trace_output()
{
  return trace_buffer_.c_str();
}

ActualCallTrace& ActualCallTrace::instance()
{
  if (instance_ == nullptr) {
    instance_ = new ActualCallTrace;
  }
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
