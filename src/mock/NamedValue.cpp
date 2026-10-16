#include "mu/tiny/mock/NamedValue.hpp"

#include "mu/tiny/mock/NamedValueComparatorsAndCopiersRepository.hpp"

#include "mu/tiny/test/Shell.hpp"

namespace mu {
namespace tiny {
namespace mock {

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

NamedValue::NamedValue(StringView name)
  : name_(name.data(), name.size())
  , type_("int")
{
  value_.int_value = 0;
}

NamedValue::NamedValue(NamedValue&& other) noexcept
  : name_(static_cast<String&&>(other.name_))
  , type_(other.type_)
  , is_const_object_(other.is_const_object_)
  , value_(other.value_)
  , size_(other.size_)
  , comparator_(other.comparator_)
  , copier_(other.copier_)
{
}

template<>
void NamedValue::set_value<bool>(bool value)
{
  type_ = "bool";
  value_.bool_value = value;
}

template<>
void NamedValue::set_value<int>(int value)
{
  type_ = "int";
  value_.int_value = value;
}

template<>
void NamedValue::set_value<unsigned int>(unsigned int value)
{
  type_ = "unsigned int";
  value_.unsigned_int_value = value;
}

template<>
void NamedValue::set_value<long int>(long int value)
{
  type_ = "long int";
  value_.long_int_value = value;
}

template<>
void NamedValue::set_value<unsigned long int>(unsigned long int value)
{
  type_ = "unsigned long int";
  value_.unsigned_long_int_value = value;
}

template<>
void NamedValue::set_value<long long>(long long value)
{
  type_ = "long long int";
  value_.long_long_int_value = value;
}

template<>
void NamedValue::set_value<unsigned long long>(unsigned long long value)
{
  type_ = "unsigned long long int";
  value_.unsigned_long_long_int_value = value;
}

template<>
void NamedValue::set_value<double>(double value)
{
  set_value(value, default_double_tolerance);
}

void NamedValue::set_value(double value, double tolerance)
{
  type_ = "double";
  value_.double_value.value = value;
  value_.double_value.tolerance = tolerance;
}

template<>
void NamedValue::set_value<void*>(void* value)
{
  type_ = "void*";
  value_.pointer_value = value;
}

template<>
void NamedValue::set_value<const void*>(const void* value)
{
  type_ = "const void*";
  value_.const_pointer_value = value;
}

template<>
void NamedValue::set_value<NamedValue::FunctionPointerValue>(
    NamedValue::FunctionPointerValue value
)
{
  type_ = "void (*)()";
  value_.function_pointer_value = value;
}

template<>
void NamedValue::set_value<const char*>(const char* value)
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
    StringView type,
    const void* object_ptr
)
{
  type_ = type;
  is_const_object_ = true;
  value_.const_object_pointer_value = object_ptr;
  if (default_repository_ != nullptr) {
    comparator_ = default_repository_->get_comparator_for_type(type);
    copier_ = default_repository_->get_copier_for_type(type);
  }
}

void NamedValue::set_object_pointer(StringView type, void* object_ptr)
{
  type_ = type;
  is_const_object_ = false;
  value_.object_pointer_value = object_ptr;
  if (default_repository_ != nullptr) {
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

const String& NamedValue::get_name() const
{
  return name_;
}

StringView NamedValue::get_type() const
{
  return type_;
}

template<>
bool NamedValue::get_value<bool>() const
{
  STRCMP_EQUAL("bool", type_.c_str());
  return value_.bool_value;
}

template<>
int NamedValue::get_value<int>() const
{
  STRCMP_EQUAL("int", type_.c_str());
  return value_.int_value;
}

template<>
unsigned int NamedValue::get_value<unsigned int>() const
{
  if (type_ == "int" && value_.int_value >= 0) {
    return static_cast<unsigned int>(value_.int_value);
  }
  STRCMP_EQUAL("unsigned int", type_.c_str());
  return value_.unsigned_int_value;
}

template<>
long int NamedValue::get_value<long int>() const
{
  if (type_ == "int") {
    return value_.int_value;
  }
  if (type_ == "unsigned int") {
    return static_cast<long int>(value_.unsigned_int_value);
  }
  STRCMP_EQUAL("long int", type_.c_str());
  return value_.long_int_value;
}

template<>
unsigned long int NamedValue::get_value<unsigned long int>() const
{
  if (type_ == "unsigned int") {
    return value_.unsigned_int_value;
  }
  if (type_ == "int" && value_.int_value >= 0) {
    return static_cast<unsigned long int>(value_.int_value);
  }
  if (type_ == "long int" && value_.long_int_value >= 0) {
    return static_cast<unsigned long int>(value_.long_int_value);
  }
  STRCMP_EQUAL("unsigned long int", type_.c_str());
  return value_.unsigned_long_int_value;
}

template<>
long long NamedValue::get_value<long long>() const
{
  if (type_ == "int") {
    return value_.int_value;
  }
  if (type_ == "unsigned int") {
    return static_cast<long long int>(value_.unsigned_int_value);
  }
  if (type_ == "long int") {
    return value_.long_int_value;
  }
  if (type_ == "unsigned long int") {
    return static_cast<long long int>(value_.unsigned_long_int_value);
  }
  STRCMP_EQUAL("long long int", type_.c_str());
  return value_.long_long_int_value;
}

template<>
unsigned long long NamedValue::get_value<unsigned long long>() const
{
  if (type_ == "unsigned int") {
    return value_.unsigned_int_value;
  }
  if (type_ == "int" && value_.int_value >= 0) {
    return static_cast<unsigned long long int>(value_.int_value);
  }
  if (type_ == "long int" && value_.long_int_value >= 0) {
    return static_cast<unsigned long long int>(value_.long_int_value);
  }
  if (type_ == "unsigned long int") {
    return value_.unsigned_long_int_value;
  }
  if (type_ == "long long int" && value_.long_long_int_value >= 0) {
    return static_cast<unsigned long long int>(value_.long_long_int_value);
  }
  STRCMP_EQUAL("unsigned long long int", type_.c_str());
  return value_.unsigned_long_long_int_value;
}

template<>
double NamedValue::get_value<double>() const
{
  STRCMP_EQUAL("double", type_.c_str());
  return value_.double_value.value;
}

double NamedValue::get_double_tolerance() const
{
  STRCMP_EQUAL("double", type_.c_str());
  return value_.double_value.tolerance;
}

template<>
const char* NamedValue::get_value<const char*>() const
{
  STRCMP_EQUAL("const char*", type_.c_str());
  return value_.string_value;
}

template<>
void* NamedValue::get_value<void*>() const
{
  STRCMP_EQUAL("void*", type_.c_str());
  return value_.pointer_value;
}

template<>
const void* NamedValue::get_value<const void*>() const
{
  STRCMP_EQUAL("const void*", type_.c_str());
  return value_.const_pointer_value;
}

template<>
NamedValue::FunctionPointerValue NamedValue::get_value<
    NamedValue::FunctionPointerValue>() const
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

bool NamedValue::equals_cross_int_long(
    const NamedValue& a,
    const NamedValue& b,
    bool& result
)
{
  if ((a.type_ == "long int") && (b.type_ == "int")) {
    result = a.value_.long_int_value == b.value_.int_value;
    return true;
  }
  if ((a.type_ == "int") && (b.type_ == "long int")) {
    result = a.value_.int_value == b.value_.long_int_value;
    return true;
  }
  if ((a.type_ == "unsigned int") && (b.type_ == "int")) {
    result = (b.value_.int_value >= 0) &&
             (a.value_.unsigned_int_value ==
              static_cast<unsigned int>(b.value_.int_value));
    return true;
  }
  if ((a.type_ == "int") && (b.type_ == "unsigned int")) {
    result = (a.value_.int_value >= 0) &&
             (static_cast<unsigned int>(a.value_.int_value) ==
              b.value_.unsigned_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long int") && (b.type_ == "int")) {
    result = (b.value_.int_value >= 0) &&
             (a.value_.unsigned_long_int_value ==
              static_cast<unsigned long>(b.value_.int_value));
    return true;
  }
  if ((a.type_ == "int") && (b.type_ == "unsigned long int")) {
    result = (a.value_.int_value >= 0) &&
             (static_cast<unsigned long>(a.value_.int_value) ==
              b.value_.unsigned_long_int_value);
    return true;
  }
  return false;
}

bool NamedValue::equals_cross_long_unsigned(
    const NamedValue& a,
    const NamedValue& b,
    bool& result
)
{
  if ((a.type_ == "unsigned int") && (b.type_ == "long int")) {
    result = (b.value_.long_int_value >= 0) &&
             (a.value_.unsigned_int_value ==
              static_cast<unsigned long>(b.value_.long_int_value));
    return true;
  }
  if ((a.type_ == "long int") && (b.type_ == "unsigned int")) {
    result = (a.value_.long_int_value >= 0) &&
             (static_cast<unsigned long>(a.value_.long_int_value) ==
              b.value_.unsigned_int_value);
    return true;
  }
  if ((a.type_ == "unsigned int") && (b.type_ == "unsigned long int")) {
    result = a.value_.unsigned_int_value == b.value_.unsigned_long_int_value;
    return true;
  }
  if ((a.type_ == "unsigned long int") && (b.type_ == "unsigned int")) {
    result = a.value_.unsigned_long_int_value == b.value_.unsigned_int_value;
    return true;
  }
  if ((a.type_ == "long int") && (b.type_ == "unsigned long int")) {
    result = (a.value_.long_int_value >= 0) &&
             (static_cast<unsigned long>(a.value_.long_int_value) ==
              b.value_.unsigned_long_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long int") && (b.type_ == "long int")) {
    result = (b.value_.long_int_value >= 0) &&
             (a.value_.unsigned_long_int_value ==
              static_cast<unsigned long>(b.value_.long_int_value));
    return true;
  }
  return false;
}

bool NamedValue::equals_cross_ll_narrow(
    const NamedValue& a,
    const NamedValue& b,
    bool& result
)
{
  if ((a.type_ == "long long int") && (b.type_ == "int")) {
    result = a.value_.long_long_int_value == b.value_.int_value;
    return true;
  }
  if ((a.type_ == "int") && (b.type_ == "long long int")) {
    result = a.value_.int_value == b.value_.long_long_int_value;
    return true;
  }
  if ((a.type_ == "long long int") && (b.type_ == "long int")) {
    result = a.value_.long_long_int_value == b.value_.long_int_value;
    return true;
  }
  if ((a.type_ == "long int") && (b.type_ == "long long int")) {
    result = a.value_.long_int_value == b.value_.long_long_int_value;
    return true;
  }
  if ((a.type_ == "long long int") && (b.type_ == "unsigned int")) {
    result = (a.value_.long_long_int_value >= 0) &&
             (static_cast<unsigned long long>(a.value_.long_long_int_value) ==
              b.value_.unsigned_int_value);
    return true;
  }
  if ((a.type_ == "unsigned int") && (b.type_ == "long long int")) {
    result = (b.value_.long_long_int_value >= 0) &&
             (a.value_.unsigned_int_value ==
              static_cast<unsigned long long>(b.value_.long_long_int_value));
    return true;
  }
  return false;
}

bool NamedValue::equals_cross_ll_wide(
    const NamedValue& a,
    const NamedValue& b,
    bool& result
)
{
  if ((a.type_ == "long long int") && (b.type_ == "unsigned long int")) {
    result = (a.value_.long_long_int_value >= 0) &&
             (static_cast<unsigned long long>(a.value_.long_long_int_value) ==
              b.value_.unsigned_long_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long int") && (b.type_ == "long long int")) {
    result = (b.value_.long_long_int_value >= 0) &&
             (a.value_.unsigned_long_int_value ==
              static_cast<unsigned long long>(b.value_.long_long_int_value));
    return true;
  }
  if ((a.type_ == "long long int") && (b.type_ == "unsigned long long int")) {
    result = (a.value_.long_long_int_value >= 0) &&
             (static_cast<unsigned long long>(a.value_.long_long_int_value) ==
              b.value_.unsigned_long_long_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long long int") && (b.type_ == "long long int")) {
    result = (b.value_.long_long_int_value >= 0) &&
             (a.value_.unsigned_long_long_int_value ==
              static_cast<unsigned long long>(b.value_.long_long_int_value));
    return true;
  }
  return false;
}

bool NamedValue::equals_cross_ull(
    const NamedValue& a,
    const NamedValue& b,
    bool& result
)
{
  if ((a.type_ == "unsigned long long int") && (b.type_ == "int")) {
    result = (b.value_.int_value >= 0) &&
             (a.value_.unsigned_long_long_int_value ==
              static_cast<unsigned long long>(b.value_.int_value));
    return true;
  }
  if ((a.type_ == "int") && (b.type_ == "unsigned long long int")) {
    result = (a.value_.int_value >= 0) &&
             (static_cast<unsigned long long>(a.value_.int_value) ==
              b.value_.unsigned_long_long_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long long int") && (b.type_ == "unsigned int")) {
    result =
        a.value_.unsigned_long_long_int_value == b.value_.unsigned_int_value;
    return true;
  }
  if ((a.type_ == "unsigned int") && (b.type_ == "unsigned long long int")) {
    result =
        a.value_.unsigned_int_value == b.value_.unsigned_long_long_int_value;
    return true;
  }
  if ((a.type_ == "unsigned long long int") && (b.type_ == "long int")) {
    result = (b.value_.long_int_value >= 0) &&
             (a.value_.unsigned_long_long_int_value ==
              static_cast<unsigned long long>(b.value_.long_int_value));
    return true;
  }
  if ((a.type_ == "long int") && (b.type_ == "unsigned long long int")) {
    result = (a.value_.long_int_value >= 0) &&
             (static_cast<unsigned long long>(a.value_.long_int_value) ==
              b.value_.unsigned_long_long_int_value);
    return true;
  }
  if ((a.type_ == "unsigned long long int") &&
      (b.type_ == "unsigned long int")) {
    result = a.value_.unsigned_long_long_int_value ==
             b.value_.unsigned_long_int_value;
    return true;
  }
  if ((a.type_ == "unsigned long int") &&
      (b.type_ == "unsigned long long int")) {
    result = a.value_.unsigned_long_int_value ==
             b.value_.unsigned_long_long_int_value;
    return true;
  }
  return false;
}

bool NamedValue::equals(const NamedValue& p) const
{
  bool eq = false;
  if (equals_cross_int_long(*this, p, eq)) {
    return eq;
  }
  if (equals_cross_long_unsigned(*this, p, eq)) {
    return eq;
  }
  if (equals_cross_ll_narrow(*this, p, eq)) {
    return eq;
  }
  if (equals_cross_ll_wide(*this, p, eq)) {
    return eq;
  }
  if (equals_cross_ull(*this, p, eq)) {
    return eq;
  }

  if (type_ != p.type_) {
    return false;
  }

  if (type_ == "bool") {
    return value_.bool_value == p.value_.bool_value;
  }
  if (type_ == "int") {
    return value_.int_value == p.value_.int_value;
  }
  if (type_ == "unsigned int") {
    return value_.unsigned_int_value == p.value_.unsigned_int_value;
  }
  if (type_ == "long int") {
    return value_.long_int_value == p.value_.long_int_value;
  }
  if (type_ == "unsigned long int") {
    return value_.unsigned_long_int_value == p.value_.unsigned_long_int_value;
  }
  if (type_ == "long long int") {
    return value_.long_long_int_value == p.value_.long_long_int_value;
  }
  if (type_ == "unsigned long long int") {
    return value_.unsigned_long_long_int_value ==
           p.value_.unsigned_long_long_int_value;
  }
  if (type_ == "const char*") {
    // NOLINTNEXTLINE(readability-container-size-empty)
    return String(value_.string_value) == String(p.value_.string_value);
  }
  if (type_ == "void*") {
    return value_.pointer_value == p.value_.pointer_value;
  }
  if (type_ == "const void*") {
    return value_.const_pointer_value == p.value_.const_pointer_value;
  }
  if (type_ == "void (*)()") {
    return value_.function_pointer_value == p.value_.function_pointer_value;
  }
  if (type_ == "double") {
    return (test::approx_equal(
        value_.double_value.value,
        p.value_.double_value.value,
        value_.double_value.tolerance
    ));
  }
  if (type_ == "const unsigned char*") {
    if (size_ != p.size_) {
      return false;
    }
    return memcmp(
               value_.memory_buffer_value, p.value_.memory_buffer_value, size_
           ) == 0;
  }

  if (comparator_ != nullptr) {
    return comparator_->is_equal(
        value_.const_object_pointer_value, p.value_.const_object_pointer_value
    );
  }

  return false;
}

bool NamedValue::compatible_for_copying(const NamedValue& p) const
{
  if ((type_ == "const void*") && (p.type_ == "void*")) {
    return true;
  }

  if ((type_ == "void*") && (p.type_ == "const void*")) {
    return true;
  }

  // with_output_parameter{,_returning} and with_captured_parameter{,_of_type}
  // both store their raw-pointer form as "void*"/"const void*", but with the
  // roles swapped between expectation and actual call (see the two checks
  // above). A same-type match here means the two sides used mismatched
  // directions (e.g. an output-parameter expectation paired with an actual
  // with_captured_parameter() call), which is never a valid pairing.
  if ((type_ == "void*") || (type_ == "const void*")) {
    return false;
  }

  return type_ == p.type_;
}

String NamedValue::to_string() const
{
  if (type_ == "bool") {
    return string_from(value_.bool_value);
  }
  if (type_ == "int") {
    return string_from(value_.int_value) + " " +
           brackets_formatted_hex_string_from(value_.int_value);
  }
  if (type_ == "unsigned int") {
    return string_from(value_.unsigned_int_value) + " " +
           brackets_formatted_hex_string_from(value_.unsigned_int_value);
  }
  if (type_ == "long int") {
    return string_from(value_.long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.long_int_value);
  }
  if (type_ == "unsigned long int") {
    return string_from(value_.unsigned_long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.unsigned_long_int_value);
  }
  if (type_ == "long long int") {
    return string_from(value_.long_long_int_value) + " " +
           brackets_formatted_hex_string_from(value_.long_long_int_value);
  }
  if (type_ == "unsigned long long int") {
    return string_from(value_.unsigned_long_long_int_value) + " " +
           brackets_formatted_hex_string_from(
               value_.unsigned_long_long_int_value
           );
  }
  if (type_ == "const char*") {
    return value_.string_value;
  }
  if (type_ == "void*") {
    return string_from(value_.pointer_value);
  }
  if (type_ == "void (*)()") {
    return string_from(value_.function_pointer_value);
  }
  if (type_ == "const void*") {
    return string_from(value_.const_pointer_value);
  }
  if (type_ == "double") {
    return string_from(value_.double_value.value);
  }
  if (type_ == "const unsigned char*") {
    return string_from_binary_with_size_or_null(
        value_.memory_buffer_value, size_
    );
  }

  if (comparator_ != nullptr) {
    return comparator_->value_to_string(value_.const_object_pointer_value);
  }

  return string_from_format(
      "No comparator found for type: \"%s\"", type_.c_str()
  );
}

} // namespace mock
} // namespace tiny
} // namespace mu
