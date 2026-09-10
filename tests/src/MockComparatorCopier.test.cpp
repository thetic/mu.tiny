#include "MockFailureReporter.hpp"

#include "mu/tiny/mock/NamedValueComparatorsAndCopiersRepository.hpp"

#include "mu/tiny/test.hpp"

using mu::tiny::mock::mock;

TEST_GROUP(MockComparatorCopier)
{
  void teardown() override
  {
    mock().check_expectations();
    mock().clear();
    mock().remove_all_comparators_and_copiers();
  }
};

namespace {
class MyTypeForTesting
{
public:
  MyTypeForTesting(long val) { value = new long(val); }
  virtual ~MyTypeForTesting() { delete value; }
  long* value;
};

class MyTypeForTestingComparator : public mu::tiny::mock::NamedValueComparator
{
public:
  bool is_equal(const void* object1, const void* object2) override
  {
    const auto* obj1 = static_cast<const MyTypeForTesting*>(object1);
    const auto* obj2 = static_cast<const MyTypeForTesting*>(object2);
    return *(obj1->value) == *(obj2->value);
  }

  mu::tiny::String value_to_string(const void* object) override
  {
    const auto* obj = static_cast<const MyTypeForTesting*>(object);
    return mu::tiny::string_from(*(obj->value));
  }
};

class MyTypeForTestingCopier : public mu::tiny::mock::NamedValueCopier
{
public:
  void copy(void* dst, const void* src) override
  {
    auto* typed_dst = static_cast<MyTypeForTesting*>(dst);
    const auto* typed_src = static_cast<const MyTypeForTesting*>(src);
    *(typed_dst->value) = *(typed_src->value);
  }
};
} // namespace

TEST(MockComparatorCopier, customObjectParameterFailsWhenNotHavingAComparisonRepository)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting object(1);
  mock()
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock()
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mu::tiny::mock::NoWayToCompareCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, customObjectParameterFailsWhenNotHavingACopierRepository)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting object(1);
  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &object
      );
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &object
      );

  mu::tiny::mock::NoWayToCopyCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, customObjectCapturedParameterFailsWhenNotHavingACopierRepository)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting object(1);
  mock()
      .expect_one_call("function")
      .with_captured_parameter_of_type(
          "MyTypeForTesting", "parameterName", &object
      );
  mock()
      .actual_call("function")
      .with_captured_parameter_of_type(
          "MyTypeForTesting", "parameterName", &object
      );

  mu::tiny::mock::NoWayToCopyCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, customObjectParameterSucceeds)
{
  MyTypeForTesting object(1);
  MyTypeForTestingComparator comparator;
  mock().install_comparator("MyTypeForTesting", comparator);

  mock()
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock()
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mock().check_expectations();

  mock().remove_all_comparators_and_copiers();
}

namespace {
bool my_type_is_equal(const void* object1, const void* object2)
{
  return static_cast<const MyTypeForTesting*>(object1)->value ==
         static_cast<const MyTypeForTesting*>(object2)->value;
}

mu::tiny::String my_type_value_to_string(const void* object)
{
  return mu::tiny::string_from(
      static_cast<const MyTypeForTesting*>(object)->value
  );
}
} // namespace

TEST(MockComparatorCopier, customObjectWithFunctionComparator)
{
  MyTypeForTesting object(1);
  mu::tiny::mock::FunctionComparator comparator(
      my_type_is_equal, my_type_value_to_string
  );
  mock().install_comparator("MyTypeForTesting", comparator);

  mock()
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock()
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mock().check_expectations();

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customObjectWithFunctionComparatorThatFailsCoversValueToString)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting object(5);
  mu::tiny::mock::FunctionComparator comparator(
      my_type_is_equal, my_type_value_to_string
  );
  mock().install_comparator("MyTypeForTesting", comparator);

  mock()
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock().check_expectations();

  STRCMP_CONTAINS("function", mock_failure_string().c_str());
}

TEST(MockComparatorCopier, customTypeOutputParameterSucceeds)
{
  MyTypeForTesting expected_object(55);
  MyTypeForTesting actual_object(99);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object
      );
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object
      );

  mock().check_expectations();
  CHECK_EQUAL(55, *(expected_object.value));
  CHECK_EQUAL(55, *(actual_object.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, noActualCallForCustomTypeOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting expected_object(1);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "output", &expected_object
  );
  mock().check_expectations();

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, unexpectedCustomTypeOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting actual_object(8834);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo");
  mock().actual_call("foo").with_output_parameter_of_type(
      "MyTypeForTesting", "parameterName", &actual_object
  );
  mock().check_expectations();

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputParameterMissing)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting expected_object(123464);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "output", &expected_object
  );
  mock().actual_call("foo");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeCapturedParameterSucceeds)
{
  MyTypeForTesting captured_object(55);
  MyTypeForTesting actual_object(99);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("function")
      .with_captured_parameter_of_type(
          "MyTypeForTesting", "parameterName", &captured_object
      );
  mock()
      .actual_call("function")
      .with_captured_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object
      );

  mock().check_expectations();
  CHECK_EQUAL(99, *(captured_object.value));
  CHECK_EQUAL(99, *(actual_object.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, noActualCallForCustomTypeCapturedParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting captured_object(1);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo").with_captured_parameter_of_type(
      "MyTypeForTesting", "captured", &captured_object
  );
  mock().check_expectations();

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, unexpectedCustomTypeCapturedParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting actual_object(8834);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo");
  mock().actual_call("foo").with_captured_parameter_of_type(
      "MyTypeForTesting", "parameterName", &actual_object
  );
  mock().check_expectations();

  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeCapturedParameterMissing)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting captured_object(123464);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo").with_captured_parameter_of_type(
      "MyTypeForTesting", "captured", &captured_object
  );
  mock().actual_call("foo");

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputParameterOfWrongType)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting expected_object(123464);
  MyTypeForTesting actual_object(75646);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock().expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "output", &expected_object
  );
  mock().actual_call("foo").with_output_parameter_of_type(
      "OtherTypeForTesting", "output", &actual_object
  );

  mock().check_expectations();
  STRCMP_CONTAINS("foo", mock_failure_string().c_str());

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, noCopierForCustomTypeOutputParameter)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting expected_object(123464);
  MyTypeForTesting actual_object(8834);

  mu::tiny::mock::NoWayToCopyCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );

  mock().expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "output", &expected_object
  );
  mock().actual_call("foo").with_output_parameter_of_type(
      "MyTypeForTesting", "output", &actual_object
  );

  mock().check_expectations();
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, twoCustomTypeOutputParameters)
{
  MyTypeForTesting expected_object1(545);
  MyTypeForTesting actual_object1(979);
  MyTypeForTesting expected_object2(123);
  MyTypeForTesting actual_object2(4567);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object1
      )
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object2
      )
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object1
      )
      .with_parameter("id", 1);
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object2
      )
      .with_parameter("id", 2);

  mock().check_expectations();
  CHECK_EQUAL(545, *(expected_object1.value));
  CHECK_EQUAL(545, *(actual_object1.value));
  CHECK_EQUAL(123, *(expected_object2.value));
  CHECK_EQUAL(123, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, twoInterleavedCustomTypeOutputParameters)
{
  MyTypeForTesting expected_object1(9545);
  MyTypeForTesting actual_object1(79);
  MyTypeForTesting expected_object2(132);
  MyTypeForTesting actual_object2(743);
  MyTypeForTestingCopier copier;

  mock().install_copier("MyTypeForTesting", copier);
  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object1
      )
      .with_parameter("id", 1);
  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object2
      )
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object2
      )
      .with_parameter("id", 2);
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object1
      )
      .with_parameter("id", 1);

  mock().check_expectations();
  CHECK_EQUAL(9545, *(expected_object1.value));
  CHECK_EQUAL(9545, *(actual_object1.value));
  CHECK_EQUAL(132, *(expected_object2.value));
  CHECK_EQUAL(132, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, twoDifferentCustomTypeOutputParametersInSameFunctionCallSucceeds)
{
  MyTypeForTesting expected_object1(11);
  MyTypeForTesting actual_object1(22);
  MyTypeForTesting expected_object2(33);
  MyTypeForTesting actual_object2(44);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("foo")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "bar", &expected_object1
      )
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "foobar", &expected_object2
      );
  mock()
      .actual_call("foo")
      .with_output_parameter_of_type("MyTypeForTesting", "bar", &actual_object1)
      .with_output_parameter_of_type(
          "MyTypeForTesting", "foobar", &actual_object2
      );

  mock().check_expectations();
  CHECK_EQUAL(11, *(expected_object1.value));
  CHECK_EQUAL(11, *(actual_object1.value));
  CHECK_EQUAL(33, *(expected_object2.value));
  CHECK_EQUAL(33, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputAndInputParametersOfSameNameInDifferentFunctionCallsOfSameFunctionSucceeds)
{
  MyTypeForTesting expected_object1(911);
  MyTypeForTesting actual_object1(6576878);
  MyTypeForTesting expected_object2(123);
  MyTypeForTesting actual_object2(123);
  MyTypeForTestingCopier copier;
  MyTypeForTestingComparator comparator;
  mock().install_copier("MyTypeForTesting", copier);
  mock().install_comparator("MyTypeForTesting", comparator);

  mock().expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "bar", &expected_object1
  );
  mock().expect_one_call("foo").with_parameter_of_type(
      "MyTypeForTesting", "bar", &expected_object2
  );
  mock().actual_call("foo").with_output_parameter_of_type(
      "MyTypeForTesting", "bar", &actual_object1
  );
  mock().actual_call("foo").with_parameter_of_type(
      "MyTypeForTesting", "bar", &actual_object2
  );

  mock().check_expectations();
  CHECK_EQUAL(911, *(expected_object1.value));
  CHECK_EQUAL(911, *(actual_object1.value));
  CHECK_EQUAL(123, *(expected_object2.value));
  CHECK_EQUAL(123, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, twoCustomTypeOutputParametersOfSameNameInDifferentFunctionsSucceeds)
{
  MyTypeForTesting expected_object1(657);
  MyTypeForTesting actual_object1(984465);
  MyTypeForTesting expected_object2(987);
  MyTypeForTesting actual_object2(987);
  MyTypeForTestingCopier copier;
  MyTypeForTestingComparator comparator;
  mock().install_copier("MyTypeForTesting", copier);
  mock().install_comparator("MyTypeForTesting", comparator);

  mock().expect_one_call("foo1").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "bar", &expected_object1
  );
  mock().expect_one_call("foo2").with_parameter_of_type(
      "MyTypeForTesting", "bar", &expected_object2
  );
  mock().actual_call("foo1").with_output_parameter_of_type(
      "MyTypeForTesting", "bar", &actual_object1
  );
  mock().actual_call("foo2").with_parameter_of_type(
      "MyTypeForTesting", "bar", &actual_object2
  );

  mock().check_expectations();
  CHECK_EQUAL(657, *(expected_object1.value));
  CHECK_EQUAL(657, *(actual_object1.value));
  CHECK_EQUAL(987, *(expected_object2.value));
  CHECK_EQUAL(987, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputAndInputParameterOfSameTypeInSameFunctionCall)
{
  MyTypeForTesting expected_object1(45);
  MyTypeForTesting actual_object1(45);
  MyTypeForTesting expected_object2(987765443);
  MyTypeForTesting actual_object2(0);
  MyTypeForTestingCopier copier;
  MyTypeForTestingComparator comparator;
  mock().install_copier("MyTypeForTesting", copier);
  mock().install_comparator("MyTypeForTesting", comparator);

  mock()
      .expect_one_call("foo")
      .with_parameter_of_type("MyTypeForTesting", "bar", &expected_object1)
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "bar", &expected_object2
      );
  mock()
      .actual_call("foo")
      .with_parameter_of_type("MyTypeForTesting", "bar", &actual_object1)
      .with_output_parameter_of_type(
          "MyTypeForTesting", "bar", &actual_object2
      );

  mock().check_expectations();
  CHECK_EQUAL(45, *(expected_object1.value));
  CHECK_EQUAL(45, *(actual_object1.value));
  CHECK_EQUAL(987765443, *(expected_object2.value));
  CHECK_EQUAL(987765443, *(actual_object2.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputParameterTraced)
{
  MyTypeForTesting actual_object(676789);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);
  mock().tracing(true);

  mock()
      .actual_call("someFunc")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "someParameter", &actual_object
      );

  mock().check_expectations();
  STRCMP_CONTAINS(
      "Function name:someFunc MyTypeForTesting someParameter:",
      mock().get_trace_output()
  );

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, customTypeOutputParameterWithIgnoredParameters)
{
  MyTypeForTesting expected_object(444537909);
  MyTypeForTesting actual_object(98765);
  MyTypeForTestingCopier copier;
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("foo")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "bar", &expected_object
      )
      .ignore_other_parameters();
  mock()
      .actual_call("foo")
      .with_output_parameter_of_type("MyTypeForTesting", "bar", &actual_object)
      .with_parameter("other", 1);

  mock().check_expectations();
  CHECK_EQUAL(444537909, *(expected_object.value));
  CHECK_EQUAL(444537909, *(actual_object.value));

  mock().remove_all_comparators_and_copiers();
}

namespace {
void my_type_copy(void* dst, const void* src)
{
  auto* typed_dst = static_cast<MyTypeForTesting*>(dst);
  const auto* typed_src = static_cast<const MyTypeForTesting*>(src);
  *(typed_dst->value) = *(typed_src->value);
}
} // namespace

TEST(MockComparatorCopier, customObjectWithFunctionCopier)
{
  MyTypeForTesting expected_object(9874452);
  MyTypeForTesting actual_object(2034);
  mu::tiny::mock::FunctionCopier copier(my_type_copy);
  mock().install_copier("MyTypeForTesting", copier);

  mock()
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &expected_object
      );
  mock()
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &actual_object
      );

  mock().check_expectations();
  CHECK_EQUAL(9874452, *(expected_object.value));
  CHECK_EQUAL(9874452, *(actual_object.value));

  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, removingComparatorsWorksHierachically)
{
  FailureReporterInstaller failure_reporter_installer;

  MyTypeForTesting object(1);
  MyTypeForTestingComparator comparator;

  mock("scope").install_comparator("MyTypeForTesting", comparator);
  mock().remove_all_comparators_and_copiers();
  mock("scope")
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock("scope")
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mu::tiny::mock::NoWayToCompareCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, removingCopiersWorksHierachically)
{
  FailureReporterInstaller failure_reporter_installer;
  MyTypeForTesting object(1);

  MyTypeForTestingCopier copier;

  mock("scope").install_copier("MyTypeForTesting", copier);
  mock().remove_all_comparators_and_copiers();
  mock("scope").expect_one_call("foo").with_output_parameter_of_type_returning(
      "MyTypeForTesting", "bar", &object
  );
  mock("scope").actual_call("foo").with_output_parameter_of_type(
      "MyTypeForTesting", "bar", &object
  );

  mu::tiny::mock::NoWayToCopyCustomTypeFailure expected_failure(
      mock_failure_test(), "MyTypeForTesting"
  );
  CHECK_EXPECTED_MOCK_FAILURE(expected_failure);
}

TEST(MockComparatorCopier, installComparatorWorksHierarchicalOnBothExistingAndDynamicallyCreatedMockSupports)
{
  MyTypeForTesting object(1);
  MyTypeForTestingComparator comparator;

  mock("existing");
  mock().install_comparator("MyTypeForTesting", comparator);
  mock("existing")
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock("existing")
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock("dynamic")
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock("dynamic")
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mock().check_expectations();
  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, installComparatorsWorksHierarchical)
{
  MyTypeForTesting object(1);
  MyTypeForTestingComparator comparator;
  mu::tiny::mock::NamedValueComparatorsAndCopiersRepository repos;
  repos.install_comparator("MyTypeForTesting", comparator);

  mock("existing");
  mock().install_comparators_and_copiers(repos);
  mock("existing")
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock("existing")
      .actual_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);

  mock().check_expectations();
  mock().remove_all_comparators_and_copiers();
}

TEST(MockComparatorCopier, installCopiersWorksHierarchically)
{
  MyTypeForTesting object(1);
  MyTypeForTestingCopier copier;

  mock("existing");
  mock().install_copier("MyTypeForTesting", copier);
  mock("existing")
      .expect_one_call("function")
      .with_output_parameter_of_type_returning(
          "MyTypeForTesting", "parameterName", &object
      );
  mock("existing")
      .actual_call("function")
      .with_output_parameter_of_type(
          "MyTypeForTesting", "parameterName", &object
      );

  mock().check_expectations();
  mock().remove_all_comparators_and_copiers();
}

namespace {
class StubComparator : public MyTypeForTestingComparator::NamedValueComparator
{
public:
  bool is_equal(const void* /*object1*/, const void* /*object2*/) override
  {
    return true;
  }
  mu::tiny::String value_to_string(const void* /*object*/) override
  {
    return "";
  }
};

struct SomeClass
{
  int some_dummy;
};

void function_with_const_param(const SomeClass param)
{
  mock()
      .actual_call("functionWithConstParam")
      .with_parameter_of_type("SomeClass", "param", &param);
}
} // namespace

TEST(MockComparatorCopier, shouldSupportConstParameters)
{
  StubComparator comparator;
  mock().install_comparator("SomeClass", comparator);

  SomeClass param;
  mock()
      .expect_one_call("functionWithConstParam")
      .with_parameter_of_type("SomeClass", "param", &param);
  function_with_const_param(param);

  mock().check_expectations();
}

TEST(MockComparatorCopier, customObjectParameterWithStringTypeAndNameArgSucceeds)
{
  MyTypeForTesting object(1);
  MyTypeForTestingComparator comparator;
  mock().install_comparator("MyTypeForTesting", comparator);

  mock()
      .expect_one_call("function")
      .with_parameter_of_type("MyTypeForTesting", "parameterName", &object);
  mock()
      .actual_call("function")
      .with_parameter_of_type(
          mu::tiny::String("MyTypeForTesting"),
          mu::tiny::String("parameterName"),
          &object
      );

  mock().check_expectations();
}
