#include "mu/tiny/mock/NamedValue.hpp"
#include "mu/tiny/mock/NamedValueComparatorsAndCopiersRepository.hpp"

#include "mu/tiny/test.hpp"

namespace {

class IntComparator : public mu::tiny::mock::NamedValueComparator
{
public:
  bool is_equal(const void* a, const void* b) override
  {
    return *static_cast<const int*>(a) == *static_cast<const int*>(b);
  }
  mu::tiny::String value_to_string(const void* a) override
  {
    return mu::tiny::string_from(*static_cast<const int*>(a));
  }
};

class IntCopier : public mu::tiny::mock::NamedValueCopier
{
public:
  void copy(void* dst, const void* src) override
  {
    *static_cast<int*>(dst) = *static_cast<const int*>(src);
  }
};

} // namespace

TEST_GROUP(NamedValue)
{
  mu::tiny::mock::NamedValue* value;
  void setup() override { value = new mu::tiny::mock::NamedValue("param"); }

  void teardown() override { delete value; }
};

TEST(NamedValue, DefaultToleranceUsedWhenNoToleranceGiven)
{
  value->set_value(0.2);
  CHECK_APPROX(
      mu::tiny::mock::NamedValue::default_double_tolerance,
      value->get_double_tolerance(),
      0.0
  );
}

TEST(NamedValue, GivenToleranceUsed)
{
  value->set_value(0.2, 3.2);
  STRCMP_EQUAL("double", value->get_type().c_str());
  CHECK_APPROX(0.2, value->get_value<double>(), 0.0);
  CHECK_APPROX(3.2, value->get_double_tolerance(), 0.0);
}

TEST(NamedValue, DoublesEqualIfWithinTolerance)
{
  value->set_value(5.0, 0.4);
  mu::tiny::mock::NamedValue other("param2");
  other.set_value(5.3);

  CHECK(value->equals(other));
}

TEST(NamedValue, DoublesNotEqualIfOutsideTolerance)
{
  value->set_value(5.0, 0.4);
  mu::tiny::mock::NamedValue other("param2");
  other.set_value(5.5);

  CHECK(!value->equals(other));
}

// ─── name / size ────────────────────────────────────────────────────────────

TEST(NamedValue, GetNameReturnsConstructorArg)
{
  STRCMP_EQUAL("param", value->get_name().c_str());
}

TEST(NamedValue, SetNameOverridesConstructorArg)
{
  value->set_name("newName");
  STRCMP_EQUAL("newName", value->get_name().c_str());
}

TEST(NamedValue, SetSizeChangesGetSize)
{
  value->set_value<int>(1);
  value->set_size(7);
  CHECK_EQUAL(size_t{ 7 }, value->get_size());
}

// ─── primitive set / get / type ─────────────────────────────────────────────

TEST(NamedValue, SetGetBool)
{
  value->set_value<bool>(true);
  STRCMP_EQUAL("bool", value->get_type().c_str());
  CHECK(value->get_value<bool>());
}

TEST(NamedValue, SetGetInt)
{
  value->set_value<int>(42);
  STRCMP_EQUAL("int", value->get_type().c_str());
  CHECK_EQUAL(42, value->get_value<int>());
}

TEST(NamedValue, SetGetUnsignedInt)
{
  value->set_value<unsigned int>(100U);
  STRCMP_EQUAL("unsigned int", value->get_type().c_str());
  CHECK_EQUAL(100U, value->get_value<unsigned int>());
}

TEST(NamedValue, SetGetLongInt)
{
  value->set_value<long int>(77L);
  STRCMP_EQUAL("long int", value->get_type().c_str());
  CHECK_EQUAL(77L, value->get_value<long int>());
}

TEST(NamedValue, SetGetUnsignedLongInt)
{
  value->set_value<unsigned long int>(888UL);
  STRCMP_EQUAL("unsigned long int", value->get_type().c_str());
  CHECK_EQUAL(888UL, value->get_value<unsigned long int>());
}

TEST(NamedValue, SetGetLongLong)
{
  value->set_value<long long>(777LL);
  STRCMP_EQUAL("long long int", value->get_type().c_str());
  CHECK_EQUAL(777LL, value->get_value<long long>());
}

TEST(NamedValue, SetGetUnsignedLongLong)
{
  value->set_value<unsigned long long>(888ULL);
  STRCMP_EQUAL("unsigned long long int", value->get_type().c_str());
  CHECK_EQUAL(888ULL, value->get_value<unsigned long long>());
}

TEST(NamedValue, SetGetConstChar)
{
  value->set_value<const char*>("hello");
  STRCMP_EQUAL("const char*", value->get_type().c_str());
  STRCMP_EQUAL("hello", value->get_value<const char*>());
}

TEST(NamedValue, SetGetVoidPointer)
{
  void* ptr = reinterpret_cast<void*>(0x123);
  value->set_value<void*>(ptr);
  STRCMP_EQUAL("void*", value->get_type().c_str());
  CHECK_EQUAL(ptr, value->get_value<void*>());
}

TEST(NamedValue, SetGetConstVoidPointer)
{
  const void* ptr = reinterpret_cast<const void*>(0x456);
  value->set_value<const void*>(ptr);
  STRCMP_EQUAL("const void*", value->get_type().c_str());
  CHECK_EQUAL(ptr, value->get_value<const void*>());
}

TEST(NamedValue, SetGetFunctionPointer)
{
  auto fp = reinterpret_cast<mu::tiny::mock::NamedValue::FunctionPointerValue>(
      0xdead
  );
  value->set_value<mu::tiny::mock::NamedValue::FunctionPointerValue>(fp);
  STRCMP_EQUAL("void (*)()", value->get_type().c_str());
  CHECK_EQUAL(
      fp, value->get_value<mu::tiny::mock::NamedValue::FunctionPointerValue>()
  );
}

// ─── memory buffer ───────────────────────────────────────────────────────────

TEST(NamedValue, SetGetMemoryBuffer)
{
  const unsigned char buf[] = { 0x01, 0x02, 0x03 };
  value->set_memory_buffer(buf, sizeof(buf));
  STRCMP_EQUAL("const unsigned char*", value->get_type().c_str());
  CHECK_EQUAL(buf, value->get_memory_buffer());
  CHECK_EQUAL(sizeof(buf), value->get_size());
}

// ─── object pointers ─────────────────────────────────────────────────────────

TEST(NamedValue, SetConstObjectPointerStoresPointerAndMarksConst)
{
  int obj = 42;
  value->set_const_object_pointer("MyType", &obj);
  STRCMP_EQUAL("MyType", value->get_type().c_str());
  CHECK_EQUAL(
      static_cast<const void*>(&obj), value->get_const_object_pointer()
  );
  CHECK(value->is_const_object());
  CHECK_EQUAL(nullptr, value->get_comparator());
  CHECK_EQUAL(nullptr, value->get_copier());
}

TEST(NamedValue, SetObjectPointerStoresPointerAndMarksNonConst)
{
  int obj = 7;
  value->set_object_pointer("MyType", &obj);
  STRCMP_EQUAL("MyType", value->get_type().c_str());
  CHECK_EQUAL(static_cast<void*>(&obj), value->get_object_pointer());
  CHECK(!value->is_const_object());
}

// ─── move constructor ────────────────────────────────────────────────────────

TEST(NamedValue, MoveConstructorTransfersNameTypeAndValue)
{
  value->set_value<int>(99);
  mu::tiny::mock::NamedValue moved(
      static_cast<mu::tiny::mock::NamedValue&&>(*value)
  );
  STRCMP_EQUAL("param", moved.get_name().c_str());
  STRCMP_EQUAL("int", moved.get_type().c_str());
  CHECK_EQUAL(99, moved.get_value<int>());
}

// ─── equals: same-type primitives ────────────────────────────────────────────

TEST(NamedValue, EqualsOnBool)
{
  value->set_value<bool>(true);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<bool>(true);
  CHECK(value->equals(other));
  other.set_value<bool>(false);
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsOnInt)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<int>(5);
  CHECK(value->equals(other));
  other.set_value<int>(6);
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsOnUnsignedInt)
{
  value->set_value<unsigned int>(5U);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned int>(5U);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnLongInt)
{
  value->set_value<long int>(5L);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long int>(5L);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnUnsignedLongInt)
{
  value->set_value<unsigned long int>(5UL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long int>(5UL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnLongLong)
{
  value->set_value<long long>(5LL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long long>(5LL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnUnsignedLongLong)
{
  value->set_value<unsigned long long>(5ULL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnConstChar)
{
  value->set_value<const char*>("hello");
  mu::tiny::mock::NamedValue other("other");
  other.set_value<const char*>("hello");
  CHECK(value->equals(other));
  other.set_value<const char*>("world");
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsOnVoidPointer)
{
  void* ptr = reinterpret_cast<void*>(0x100);
  value->set_value<void*>(ptr);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<void*>(ptr);
  CHECK(value->equals(other));
  other.set_value<void*>(nullptr);
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsOnConstVoidPointer)
{
  const void* ptr = reinterpret_cast<const void*>(0x100);
  value->set_value<const void*>(ptr);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<const void*>(ptr);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnFunctionPointer)
{
  auto fp = reinterpret_cast<mu::tiny::mock::NamedValue::FunctionPointerValue>(
      0xdead
  );
  value->set_value<mu::tiny::mock::NamedValue::FunctionPointerValue>(fp);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<mu::tiny::mock::NamedValue::FunctionPointerValue>(fp);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnMemoryBufferSameContents)
{
  const unsigned char buf[] = { 0x01, 0x02 };
  value->set_memory_buffer(buf, sizeof(buf));
  mu::tiny::mock::NamedValue other("other");
  other.set_memory_buffer(buf, sizeof(buf));
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsOnMemoryBufferDifferentSize)
{
  const unsigned char buf[] = { 0x01, 0x02 };
  value->set_memory_buffer(buf, 2);
  mu::tiny::mock::NamedValue other("other");
  other.set_memory_buffer(buf, 1);
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsTypeMismatchReturnsFalse)
{
  value->set_value<bool>(false);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<const char*>("false");
  CHECK(!value->equals(other));
}

TEST(NamedValue, EqualsObjectTypeWithoutComparatorReturnsFalse)
{
  int obj = 1;
  value->set_const_object_pointer("MyType", &obj);
  mu::tiny::mock::NamedValue other("other");
  other.set_const_object_pointer("MyType", &obj);
  CHECK(!value->equals(other));
}

// ─── equals: cross-type integer reverse directions ───────────────────────────
//
// MockParameter.test.cpp covers all 30 cross-type pairs, but always with
// expected.equals(actual).  Each pair has a corresponding "reverse" branch in
// the if-else chain that is only reached when this==actual and p==expected.
// The tests below exercise those complementary branches.

TEST(NamedValue, EqualsIntAgainstLongInt)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long int>(5L);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsIntAgainstUnsignedInt)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned int>(5U);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsIntAgainstUnsignedLongInt)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long int>(5UL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedIntAgainstLongInt)
{
  value->set_value<unsigned int>(5U);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long int>(5L);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedIntAgainstUnsignedLongInt)
{
  value->set_value<unsigned int>(5U);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long int>(5UL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsLongIntAgainstUnsignedLongInt)
{
  value->set_value<long int>(5L);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long int>(5UL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsIntAgainstLongLong)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long long>(5LL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsLongIntAgainstLongLong)
{
  value->set_value<long int>(5L);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long long>(5LL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedIntAgainstLongLong)
{
  value->set_value<unsigned int>(5U);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long long>(5LL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedLongIntAgainstLongLong)
{
  value->set_value<unsigned long int>(5UL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<long long>(5LL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsLongLongAgainstUnsignedLongLong)
{
  value->set_value<long long>(5LL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsIntAgainstUnsignedLongLong)
{
  value->set_value<int>(5);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedIntAgainstUnsignedLongLong)
{
  value->set_value<unsigned int>(5U);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsLongIntAgainstUnsignedLongLong)
{
  value->set_value<long int>(5L);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

TEST(NamedValue, EqualsUnsignedLongIntAgainstUnsignedLongLong)
{
  value->set_value<unsigned long int>(5UL);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<unsigned long long>(5ULL);
  CHECK(value->equals(other));
}

// ─── to_string ───────────────────────────────────────────────────────────────

TEST(NamedValue, ToStringBool)
{
  value->set_value<bool>(true);
  STRCMP_EQUAL("true", value->to_string().c_str());
}

TEST(NamedValue, ToStringInt)
{
  value->set_value<int>(10);
  STRCMP_EQUAL("10 (0xa)", value->to_string().c_str());
}

TEST(NamedValue, ToStringUnsignedInt)
{
  value->set_value<unsigned int>(7U);
  STRCMP_EQUAL("7 (0x7)", value->to_string().c_str());
}

TEST(NamedValue, ToStringLongInt)
{
  value->set_value<long int>(777L);
  STRCMP_CONTAINS("777", value->to_string().c_str());
}

TEST(NamedValue, ToStringUnsignedLongInt)
{
  value->set_value<unsigned long int>(888UL);
  STRCMP_CONTAINS("888", value->to_string().c_str());
}

TEST(NamedValue, ToStringLongLong)
{
  value->set_value<long long>(777LL);
  STRCMP_CONTAINS("777", value->to_string().c_str());
}

TEST(NamedValue, ToStringUnsignedLongLong)
{
  value->set_value<unsigned long long>(888ULL);
  STRCMP_CONTAINS("888", value->to_string().c_str());
}

TEST(NamedValue, ToStringConstChar)
{
  value->set_value<const char*>("hello world");
  STRCMP_EQUAL("hello world", value->to_string().c_str());
}

TEST(NamedValue, ToStringVoidPointer)
{
  value->set_value<void*>(reinterpret_cast<void*>(0x100));
  STRCMP_CONTAINS("0x100", value->to_string().c_str());
}

TEST(NamedValue, ToStringFunctionPointer)
{
  value->set_value<mu::tiny::mock::NamedValue::FunctionPointerValue>(
      reinterpret_cast<mu::tiny::mock::NamedValue::FunctionPointerValue>(0x100)
  );
  STRCMP_CONTAINS("0x100", value->to_string().c_str());
}

TEST(NamedValue, ToStringConstVoidPointer)
{
  value->set_value<const void*>(reinterpret_cast<const void*>(0x200));
  STRCMP_CONTAINS("0x200", value->to_string().c_str());
}

TEST(NamedValue, ToStringDouble)
{
  value->set_value(3.14, 0.0);
  STRCMP_CONTAINS("3.14", value->to_string().c_str());
}

TEST(NamedValue, ToStringMemoryBuffer)
{
  const unsigned char buf[] = { 0xAB, 0xCD };
  value->set_memory_buffer(buf, sizeof(buf));
  STRCMP_CONTAINS("AB CD", value->to_string().c_str());
}

TEST(NamedValue, ToStringObjectWithoutComparatorReportsTypeName)
{
  int obj = 1;
  value->set_const_object_pointer("MyType", &obj);
  STRCMP_EQUAL(
      "No comparator found for type: \"MyType\"", value->to_string().c_str()
  );
}

// ─── compatible_for_copying ──────────────────────────────────────────────────

TEST(NamedValue, CompatibleForCopyingWhenSameType)
{
  value->set_value<int>(1);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<int>(2);
  CHECK(value->compatible_for_copying(other));
}

TEST(NamedValue, CompatibleForCopyingConstVoidAgainstVoid)
{
  value->set_value<const void*>(nullptr);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<void*>(nullptr);
  CHECK(value->compatible_for_copying(other));
}

TEST(NamedValue, CompatibleForCopyingVoidAgainstConstVoid)
{
  value->set_value<void*>(nullptr);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<const void*>(nullptr);
  CHECK(value->compatible_for_copying(other));
}

TEST(NamedValue, NotCompatibleForCopyingDifferentTypes)
{
  value->set_value<int>(1);
  mu::tiny::mock::NamedValue other("other");
  other.set_value<const char*>("hello");
  CHECK(!value->compatible_for_copying(other));
}

// ─── repository ──────────────────────────────────────────────────────────────

TEST_GROUP(NamedValueRepository)
{
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository* saved_repository;
  void setup() override
  {
    saved_repository = mu::tiny::mock::NamedValue::
        get_default_comparators_and_copiers_repository();
  }
  void teardown() override
  {
    mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
        saved_repository
    );
  }
};

TEST(NamedValueRepository, SetAndGetDefaultRepository)
{
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repository;
  mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
      &repository
  );
  CHECK_EQUAL(
      &repository,
      mu::tiny::mock::NamedValue::
          get_default_comparators_and_copiers_repository()
  );
}

TEST(NamedValueRepository, ComparatorAndCopierAttachedViaDefaultRepository)
{
  IntComparator comparator;
  IntCopier copier;
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repository;
  repository.install_comparator("int_obj", comparator);
  repository.install_copier("int_obj", copier);
  mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
      &repository
  );

  int obj = 5;
  mu::tiny::mock::NamedValue named("v");
  named.set_const_object_pointer("int_obj", &obj);
  CHECK_EQUAL(&comparator, named.get_comparator());
  CHECK_EQUAL(&copier, named.get_copier());
}

TEST(NamedValueRepository, ObjectPointerAlsoLooksUpComparatorAndCopier)
{
  IntComparator comparator;
  IntCopier copier;
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repository;
  repository.install_comparator("int_obj", comparator);
  repository.install_copier("int_obj", copier);
  mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
      &repository
  );

  int obj = 5;
  mu::tiny::mock::NamedValue named("v");
  named.set_object_pointer("int_obj", &obj);
  CHECK_EQUAL(&comparator, named.get_comparator());
  CHECK_EQUAL(&copier, named.get_copier());
}

TEST(NamedValueRepository, EqualsObjectTypeWithComparator)
{
  IntComparator comparator;
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repository;
  repository.install_comparator("int_obj", comparator);
  mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
      &repository
  );

  int obj1 = 5;
  mu::tiny::mock::NamedValue v1("v1");
  v1.set_const_object_pointer("int_obj", &obj1);

  int obj2 = 5;
  mu::tiny::mock::NamedValue v2("v2");
  v2.set_const_object_pointer("int_obj", &obj2);

  int obj3 = 6;
  mu::tiny::mock::NamedValue v3("v3");
  v3.set_const_object_pointer("int_obj", &obj3);

  CHECK(v1.equals(v2));
  CHECK(!v1.equals(v3));
}

TEST(NamedValueRepository, ToStringObjectTypeWithComparator)
{
  IntComparator comparator;
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repository;
  repository.install_comparator("int_obj", comparator);
  mu::tiny::mock::NamedValue::set_default_comparators_and_copiers_repository(
      &repository
  );

  int obj = 42;
  mu::tiny::mock::NamedValue named("v");
  named.set_const_object_pointer("int_obj", &obj);
  STRCMP_EQUAL("42", named.to_string().c_str());
}
