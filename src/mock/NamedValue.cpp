#include "mutiny/mock/NamedValue.hpp"

#include "mutiny/mock/NamedValueComparatorsAndCopiersRepository.hpp"

#include "mutiny/test/Shell.hpp"

namespace mu {
namespace tiny {
namespace mock {
using namespace mu::tiny::test;

NamedValueComparatorsAndCopiersRepository* NamedValue::default_repository_ =
    nullptr;
const double NamedValue::default_double_tolerance = 0.005;

void NamedValue::set_default_comparators_and_copiers_repository(
    NamedValueComparatorsAndCopiersRepository* repository
)
{
  default_repository_ = repository;
}

NamedValueComparatorsAndCopiersRepository* NamedValue::
    get_default_comparators_and_copiers_repository()
{
  return default_repository_;
}

NamedValue::NamedValue(const String& name)
  : name_(name)
  , type_("int")

{
  value_.int_value = 0;
}

NamedValue::NamedValue(NamedValue&& other) noexcept
  : name_(static_cast<String&&>(other.name_))
  , type_(static_cast<String&&>(other.type_))
  , is_const_object_(other.is_const_object_)
  , value_(other.value_)
  , size_(other.size_)
  , comparator_(other.comparator_)
  , copier_(other.copier_)
{
}

void NamedValue::set_value(bool value)
{
  type_ = "bool";
  value_.bool_value = value;
}

void NamedValue::set_value(unsigned int value)
{
  type_ = "unsigned int";
  value_.unsigned_int_value = value;
}

void NamedValue::set_value(int value)
{
  type_ = "int";
  value_.int_value = value;
}

void NamedValue::set_value(long int value)
{
  type_ = "long int";
  value_.long_int_value = value;
}

void NamedValue::set_value(unsigned long int value)
{
  type_ = "unsigned long int";
  value_.unsigned_long_int_value = value;
}

void NamedValue::set_value(long long value)
{
  type_ = "long long int";
  value_.long_long_int_value = value;
}

void NamedValue::set_value(unsigned long long value)
{
  type_ = "unsigned long long int";
  value_.unsigned_long_long_int_value = value;
}

void NamedValue::set_value(double value)
{
  set_value(value, default_double_tolerance);
}

void NamedValue::set_value(double value, double tolerance)
{
  type_ = "double";
  value_.double_value.value = value;
  value_.double_value.tolerance = tolerance;
}

void NamedValue::set_value(void* value)
{
  type_ = "void*";
  value_.pointer_value = value;
}

void NamedValue::set_value(const void* value)
{
  type_ = "const void*";
  value_.const_pointer_value = value;
}

void NamedValue::set_value(NamedValue::FunctionPointerValue value)
{
  type_ = "void (*)()";
  value_.function_pointer_value = value;
}

void NamedValue::set_value(const char* value)
{
  type_ = "const char*";
  value_.string_value = value;
}

void NamedValue::set_memory_buffer(const unsigned char* value, size_t size)
{
  type_ = "const unsigned char*";
  value_.memory_buffer_value = value;
  size_ = size;
}

void NamedValue::set_const_object_pointer(
    const String& type,
    const void* object_ptr
)
{
  type_ = type;
  is_const_object_ = true;
  value_.const_object_pointer_value = object_ptr;
  if (default_repository_) {
    comparator_ = default_repository_->get_comparator_for_type(type);
    copier_ = default_repository_->get_copier_for_type(type);
  }
}

void NamedValue::set_object_pointer(const String& type, void* object_ptr)
{
  type_ = type;
  is_const_object_ = false;
  value_.object_pointer_value = object_ptr;
  if (default_repository_) {
    comparator_ = default_repository_->get_comparator_for_type(type);
    copier_ = default_repository_->get_copier_for_type(type);
  }
}

void NamedValue::set_size(size_t size)
{
  size_ = size;
}

void NamedValue::set_name(const char* name)
{
  name_ = name;
}

String NamedValue::get_name() const
{
  return name_;
}

String NamedValue::get_type() const
{
  return type_;
}

bool NamedValue::get_bool_value() const
{
  STRCMP_EQUAL("bool", type_.c_str());
  return value_.bool_value;
}

unsigned int NamedValue::get_unsigned_int_value() const
{
  if (type_ == "int" && value_.int_value >= 0)
    return static_cast<unsigned int>(value_.int_value);
  else {
    STRCMP_EQUAL("unsigned int", type_.c_str());
    return value_.unsigned_int_value;
  }
}

int NamedValue::get_int_value() const
{
  STRCMP_EQUAL("int", type_.c_str());
  return value_.int_value;
}

long int NamedValue::get_long_int_value() const
{
  if (type_ == "int")
    return value_.int_value;
  else if (type_ == "unsigned int")
    return static_cast<long int>(value_.unsigned_int_value);
  else {
    STRCMP_EQUAL("long int", type_.c_str());
    return value_.long_int_value;
  }
}

unsigned long int NamedValue::get_unsigned_long_int_value() const
{
  if (type_ == "unsigned int")
    return value_.unsigned_int_value;
  else if (type_ == "int" && value_.int_value >= 0)
    return static_cast<unsigned long int>(value_.int_value);
  else if (type_ == "long int" && value_.long_int_value >= 0)
    return static_cast<unsigned long int>(value_.long_int_value);
  else {
    STRCMP_EQUAL("unsigned long int", type_.c_str());
    return value_.unsigned_long_int_value;
  }
}

long long NamedValue::get_long_long_int_value() const
{
  if (type_ == "int")
    return value_.int_value;
  else if (type_ == "unsigned int")
    return static_cast<long long int>(value_.unsigned_int_value);
  else if (type_ == "long int")
    return value_.long_int_value;
  else if (type_ == "unsigned long int")
    return static_cast<long long int>(value_.unsigned_long_int_value);
  else {
    STRCMP_EQUAL("long long int", type_.c_str());
    return value_.long_long_int_value;
  }
}

unsigned long long NamedValue::get_unsigned_long_long_int_value() const
{
  if (type_ == "unsigned int")
    return value_.unsigned_int_value;
  else if (type_ == "int" && value_.int_value >= 0)
    return static_cast<unsigned long long int>(value_.int_value);
  else if (type_ == "long int" && value_.long_int_value >= 0)
    return static_cast<unsigned long long int>(value_.long_int_value);
  else if (type_ == "unsigned long int")
    return value_.unsigned_long_int_value;
  else if (type_ == "long long int" && value_.long_long_int_value >= 0)
    return static_cast<unsigned long long int>(value_.long_long_int_value);
  else {
    STRCMP_EQUAL("unsigned long long int", type_.c_str());
    return value_.unsigned_long_long_int_value;
  }
}

double NamedValue::get_double_value() const
{
  STRCMP_EQUAL("double", type_.c_str());
  return value_.double_value.value;
}

double NamedValue::get_double_tolerance() const
{
  STRCMP_EQUAL("double", type_.c_str());
  return value_.double_value.tolerance;
}

const char* NamedValue::get_string_value() const
{
  STRCMP_EQUAL("const char*", type_.c_str());
  return value_.string_value;
}

void* NamedValue::get_pointer_value() const
{
  STRCMP_EQUAL("void*", type_.c_str());
  return value_.pointer_value;
}

const void* NamedValue::get_const_pointer_value() const
{
  STRCMP_EQUAL("const void*", type_.c_str());
  return value_.pointer_value;
}

NamedValue::FunctionPointerValue NamedValue::get_function_pointer_value() const
{
  STRCMP_EQUAL("void (*)()", type_.c_str());
  return value_.function_pointer_value;
}

const unsigned char* NamedValue::get_memory_buffer() const
{
  STRCMP_EQUAL("const unsigned char*", type_.c_str());
  return value_.memory_buffer_value;
}

const void* NamedValue::get_const_object_pointer() const
{
  return value_.const_object_pointer_value;
}

void* NamedValue::get_object_pointer() const
{
  return value_.object_pointer_value;
}

bool NamedValue::is_const_object() const
{
  return is_const_object_;
}

size_t NamedValue::get_size() const
{
  return size_;
}

NamedValueComparator* NamedValue::get_comparator() const
{
  return comparator_;
}

NamedValueCopier* NamedValue::get_copier() const
{
  return copier_;
}

bool NamedValue::equals(const NamedValue& p) const
{
  if ((type_ == "long int") && (p.type_ == "int"))
    return value_.long_int_value == p.value_.int_value;
  else if ((type_ == "int") && (p.type_ == "long int"))
    return value_.int_value == p.value_.long_int_value;
  else if ((type_ == "unsigned int") && (p.type_ == "int"))
    return (p.value_.int_value >= 0) &&
           (value_.unsigned_int_value ==
            static_cast<unsigned int>(p.value_.int_value));
  else if ((type_ == "int") && (p.type_ == "unsigned int"))
    return (value_.int_value >= 0) &&
           (static_cast<unsigned int>(value_.int_value) ==
            p.value_.unsigned_int_value);
  else if ((type_ == "unsigned long int") && (p.type_ == "int"))
    return (p.value_.int_value >= 0) &&
           (value_.unsigned_long_int_value ==
            static_cast<unsigned long>(p.value_.int_value));
  else if ((type_ == "int") && (p.type_ == "unsigned long int"))
    return (value_.int_value >= 0) &&
           (static_cast<unsigned long>(value_.int_value) ==
            p.value_.unsigned_long_int_value);
  else if ((type_ == "unsigned int") && (p.type_ == "long int"))
    return (p.value_.long_int_value >= 0) &&
           (value_.unsigned_int_value ==
            static_cast<unsigned long>(p.value_.long_int_value));
  else if ((type_ == "long int") && (p.type_ == "unsigned int"))
    return (value_.long_int_value >= 0) &&
           (static_cast<unsigned long>(value_.long_int_value) ==
            p.value_.unsigned_int_value);
  else if ((type_ == "unsigned int") && (p.type_ == "unsigned long int"))
    return value_.unsigned_int_value == p.value_.unsigned_long_int_value;
  else if ((type_ == "unsigned long int") && (p.type_ == "unsigned int"))
    return value_.unsigned_long_int_value == p.value_.unsigned_int_value;
  else if ((type_ == "long int") && (p.type_ == "unsigned long int"))
    return (value_.long_int_value >= 0) &&
           (static_cast<unsigned long>(value_.long_int_value) ==
            p.value_.unsigned_long_int_value);
  else if ((type_ == "unsigned long int") && (p.type_ == "long int"))
    return (p.value_.long_int_value >= 0) &&
           (value_.unsigned_long_int_value ==
            static_cast<unsigned long>(p.value_.long_int_value));
  else if ((type_ == "long long int") && (p.type_ == "int"))
    return value_.long_long_int_value == p.value_.int_value;
  else if ((type_ == "int") && (p.type_ == "long long int"))
    return value_.int_value == p.value_.long_long_int_value;
  else if ((type_ == "long long int") && (p.type_ == "long int"))
    return value_.long_long_int_value == p.value_.long_int_value;
  else if ((type_ == "long int") && (p.type_ == "long long int"))
    return value_.long_int_value == p.value_.long_long_int_value;
  else if ((type_ == "long long int") && (p.type_ == "unsigned int"))
    return (value_.long_long_int_value >= 0) &&
           (static_cast<unsigned long long>(value_.long_long_int_value) ==
            p.value_.unsigned_int_value);
  else if ((type_ == "unsigned int") && (p.type_ == "long long int"))
    return (p.value_.long_long_int_value >= 0) &&
           (value_.unsigned_int_value ==
            static_cast<unsigned long long>(p.value_.long_long_int_value));
  else if ((type_ == "long long int") && (p.type_ == "unsigned long int"))
    return (value_.long_long_int_value >= 0) &&
           (static_cast<unsigned long long>(value_.long_long_int_value) ==
            p.value_.unsigned_long_int_value);
  else if ((type_ == "unsigned long int") && (p.type_ == "long long int"))
    return (p.value_.long_long_int_value >= 0) &&
           (value_.unsigned_long_int_value ==
            static_cast<unsigned long long>(p.value_.long_long_int_value));
  else if ((type_ == "long long int") && (p.type_ == "unsigned long long int"))
    return (value_.long_long_int_value >= 0) &&
           (static_cast<unsigned long long>(value_.long_long_int_value) ==
            p.value_.unsigned_long_long_int_value);
  else if ((type_ == "unsigned long long int") && (p.type_ == "long long int"))
    return (p.value_.long_long_int_value >= 0) &&
           (value_.unsigned_long_long_int_value ==
            static_cast<unsigned long long>(p.value_.long_long_int_value));
  else if ((type_ == "unsigned long long int") && (p.type_ == "int"))
    return (p.value_.int_value >= 0) &&
           (value_.unsigned_long_long_int_value ==
            static_cast<unsigned long long>(p.value_.int_value));
  else if ((type_ == "int") && (p.type_ == "unsigned long long int"))
    return (value_.int_value >= 0) &&
           (static_cast<unsigned long long>(value_.int_value) ==
            p.value_.unsigned_long_long_int_value);
  else if ((type_ == "unsigned long long int") && (p.type_ == "unsigned int"))
    return value_.unsigned_long_long_int_value == p.value_.unsigned_int_value;
  else if ((type_ == "unsigned int") && (p.type_ == "unsigned long long int"))
    return value_.unsigned_int_value == p.value_.unsigned_long_long_int_value;
  else if ((type_ == "unsigned long long int") && (p.type_ == "long int"))
    return (p.value_.long_int_value >= 0) &&
           (value_.unsigned_long_long_int_value ==
            static_cast<unsigned long long>(p.value_.long_int_value));
  else if ((type_ == "long int") && (p.type_ == "unsigned long long int"))
    return (value_.long_int_value >= 0) &&
           (static_cast<unsigned long long>(value_.long_int_value) ==
            p.value_.unsigned_long_long_int_value);
  else if ((type_ == "unsigned long long int") &&
           (p.type_ == "unsigned long int"))
    return value_.unsigned_long_long_int_value ==
           p.value_.unsigned_long_int_value;
  else if ((type_ == "unsigned long int") &&
           (p.type_ == "unsigned long long int"))
    return value_.unsigned_long_int_value ==
           p.value_.unsigned_long_long_int_value;

  if (type_ != p.type_)
    return false;

  if (type_ == "bool")
    return value_.bool_value == p.value_.bool_value;
  else if (type_ == "int")
    return value_.int_value == p.value_.int_value;
  else if (type_ == "unsigned int")
    return value_.unsigned_int_value == p.value_.unsigned_int_value;
  else if (type_ == "long int")
    return value_.long_int_value == p.value_.long_int_value;
  else if (type_ == "unsigned long int")
    return value_.unsigned_long_int_value == p.value_.unsigned_long_int_value;
  else if (type_ == "long long int")
    return value_.long_long_int_value == p.value_.long_long_int_value;
  else if (type_ == "unsigned long long int")
    return value_.unsigned_long_long_int_value ==
           p.value_.unsigned_long_long_int_value;
  else if (type_ == "const char*")
    return String(value_.string_value) == String(p.value_.string_value);
  else if (type_ == "void*")
    return value_.pointer_value == p.value_.pointer_value;
  else if (type_ == "const void*")
    return value_.const_pointer_value == p.value_.const_pointer_value;
  else if (type_ == "void (*)()")
    return value_.function_pointer_value == p.value_.function_pointer_value;
  else if (type_ == "double")
    return (doubles_equal(
        value_.double_value.value,
        p.value_.double_value.value,
        value_.double_value.tolerance
    ));
  else if (type_ == "const unsigned char*") {
    if (size_ != p.size_) {
      return false;
    }
    return memcmp(
               value_.memory_buffer_value, p.value_.memory_buffer_value, size_
           ) == 0;
  }

  if (comparator_)
    return comparator_->is_equal(
        value_.const_object_pointer_value, p.value_.const_object_pointer_value
    );

  return false;
}

bool NamedValue::compatible_for_copying(const NamedValue& p) const
{
  if (type_ == p.type_)
    return true;

  if ((type_ == "const void*") && (p.type_ == "void*"))
    return true;

  return false;
}

String NamedValue::to_string() const
{
  if (type_ == "bool")
    return string_from(value_.bool_value);
  else if (type_ == "int")
    return string_from(value_.int_value) + " " +
           brackets_formatted_hex_string_from(value_.int_value);
  else if (type_ == "unsigned int")
    return string_from(value_.unsigned_int_value) + " " +
           brackets_formatted_hex_string_from(value_.unsigned_int_value);
  else if (type_ == "long int")
    return string_from(value_.long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.long_int_value);
  else if (type_ == "unsigned long int")
    return string_from(value_.unsigned_long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.unsigned_long_int_value);
  else if (type_ == "long long int")
    return string_from(value_.long_long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.long_long_int_value);
  else if (type_ == "unsigned long long int")
    return string_from(value_.unsigned_long_long_int_value) + " " +
           brackets_formatted_hex_string_from(
               value_.unsigned_long_long_int_value
           );
  else if (type_ == "const char*")
    return value_.string_value;
  else if (type_ == "void*")
    return string_from(value_.pointer_value);
  else if (type_ == "void (*)()")
    return string_from(value_.function_pointer_value);
  else if (type_ == "const void*")
    return string_from(value_.const_pointer_value);
  else if (type_ == "double")
    return string_from(value_.double_value.value);
  else if (type_ == "const unsigned char*")
    return string_from_binary_with_size_or_null(
        value_.memory_buffer_value, size_
    );

  if (comparator_)
    return comparator_->value_to_string(value_.const_object_pointer_value);

  return string_from_format(
      "No comparator found for type: \"%s\"", type_.c_str()
  );
}

} // namespace mock
} // namespace tiny
} // namespace mu
