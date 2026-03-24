Testing C Code
==============

mutiny supports writing tests in pure C. Because the test runner itself
is C++, you need two files per test group: a ``.c`` file containing
the tests, and a ``.cpp`` wrapper that registers them with the C++
runner.

The Two-File Pattern
--------------------

The C test file
~~~~~~~~~~~~~~~

Include ``"mutiny/test.h"`` and use the C macros:

.. literalinclude:: ../examples/tests/hello.test.c
   :language: c

``TEST_GROUP_SETUP`` and ``TEST_GROUP_TEARDOWN`` in C define wrapper
functions that the ``.cpp`` file calls. ``TEST`` in C defines a
wrapper function body.

The C++ registration wrapper
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../examples/tests/hello.test.cpp
   :language: cpp

Both files must be compiled together into the same test executable:

.. code-block:: cmake

   add_executable(my_tests main.cpp foo_test.c foo_test.cpp)
   target_link_libraries(my_tests PRIVATE mu::tiny)

C Assertion Macros (``mutiny/test.h``)
--------------------------------------

The C header provides typed macros because C does not have overloaded
functions:

.. list-table::
   :header-rows: 1

   * - Macro
     - Type checked
   * - :c:macro:`CHECK(condition) <CHECK>`
     - any non-zero value
   * - :c:macro:`CHECK_TEXT(condition, text) <CHECK_TEXT>`
     - same with message
   * - :c:macro:`CHECK_EQUAL_BOOL(expected, actual) <CHECK_EQUAL_BOOL>`
     - ``bool``
   * - :c:macro:`CHECK_EQUAL_INT(expected, actual) <CHECK_EQUAL_INT>`
     - ``int``
   * - :c:macro:`CHECK_EQUAL_UINT(expected, actual) <CHECK_EQUAL_UINT>`
     - ``unsigned int``
   * - :c:macro:`CHECK_EQUAL_LONG(expected, actual) <CHECK_EQUAL_LONG>`
     - ``long``
   * - :c:macro:`CHECK_EQUAL_ULONG(expected, actual) <CHECK_EQUAL_ULONG>`
     - ``unsigned long``
   * - :c:macro:`CHECK_EQUAL_LONGLONG(expected, actual) <CHECK_EQUAL_LONGLONG>`
     - ``long long``
   * - :c:macro:`CHECK_EQUAL_ULONGLONG(expected, actual) <CHECK_EQUAL_ULONGLONG>`
     - ``unsigned long long``
   * - :c:macro:`CHECK_EQUAL_DOUBLE(expected, actual, threshold) <CHECK_EQUAL_DOUBLE>`
     - ``double`` within ±threshold
   * - :c:macro:`CHECK_EQUAL_CHAR(expected, actual) <CHECK_EQUAL_CHAR>`
     - ``char``
   * - :c:macro:`CHECK_EQUAL_UBYTE(expected, actual) <CHECK_EQUAL_UBYTE>`
     - ``unsigned char``
   * - :c:macro:`CHECK_EQUAL_SBYTE(expected, actual) <CHECK_EQUAL_SBYTE>`
     - ``signed char``
   * - :c:macro:`CHECK_EQUAL_STRING(expected, actual) <CHECK_EQUAL_STRING>`
     - ``const char*`` via ``strcmp``
   * - :c:macro:`CHECK_EQUAL_POINTER(expected, actual) <CHECK_EQUAL_POINTER>`
     - ``const void*``
   * - :c:macro:`CHECK_EQUAL_MEMCMP(expected, actual, size) <CHECK_EQUAL_MEMCMP>`
     - raw memory via ``memcmp``
   * - :c:macro:`FAIL() <FAIL>`
     - unconditional failure (no message)
   * - :c:macro:`FAIL_TEXT(text) <FAIL_TEXT>`
     - unconditional failure with message
   * - :c:macro:`TEST_PROPERTY(name, value) <TEST_PROPERTY>`
     - attaches metadata to :doc:`JUnit output <junit-output>`

Each macro also has a ``_TEXT`` variant that accepts an extra message
string.

C Test Definition Macros
-------------------------

.. list-table::
   :header-rows: 1

   * - Macro
     - Purpose
   * - :c:macro:`TEST_GROUP_SETUP(group) <TEST_GROUP_SETUP>`
     - Defines the setup function for group
   * - :c:macro:`TEST_GROUP_TEARDOWN(group) <TEST_GROUP_TEARDOWN>`
     - Defines the teardown function for group
   * - :c:macro:`TEST(group, name) <TEST>`
     - Defines a test function body
   * - :c:macro:`IGNORE_TEST(group, name) <IGNORE_TEST>`
     - Defines an ignored test body
   * - :c:macro:`EXPECT_FAIL_TEST(group, name) <EXPECT_FAIL_TEST>`
     - Defines an expected-failure test body

C++ Wrapper Macros
------------------

Each C macro that defines a test body has a matching C++ wrapper:

.. list-table::
   :header-rows: 1

   * - C++ wrapper
     - What it creates
   * - :c:macro:`TEST_GROUP_C_WRAPPER(group) <TEST_GROUP_C_WRAPPER>`
     - A :c:macro:`TEST_GROUP` with optional setup/teardown wiring
   * - :c:macro:`TEST_GROUP_C_SETUP_WRAPPER(group) <TEST_GROUP_C_SETUP_WRAPPER>`
     - Calls the C setup function from C++ ``setup()``
   * - :c:macro:`TEST_GROUP_C_TEARDOWN_WRAPPER(group) <TEST_GROUP_C_TEARDOWN_WRAPPER>`
     - Calls the C teardown function from C++ ``teardown()``
   * - :c:macro:`TEST_C_WRAPPER(group, name) <TEST_C_WRAPPER>`
     - A :c:macro:`TEST` that calls the C test function
   * - :c:macro:`IGNORE_TEST_C_WRAPPER(group, name) <IGNORE_TEST_C_WRAPPER>`
     - An :c:macro:`IGNORE_TEST` that calls the C ignored function
   * - :c:macro:`EXPECT_FAIL_TEST_C_WRAPPER(group, name) <EXPECT_FAIL_TEST_C_WRAPPER>`
     - An :c:macro:`EXPECT_FAIL_TEST` that calls the C expected-fail function

C Mock Interface (``mutiny/mock.h``)
-------------------------------------

.. c:struct:: MutinyMockSupport

   Vtable of function pointers that form the C mock API.  Obtain a
   pointer via :c:func:`mutiny_mock` or :c:func:`mutiny_mock_scope`.

.. c:function:: MutinyMockSupport* mutiny_mock(void)

   Returns the global :c:struct:`MutinyMockSupport` instance.

.. c:function:: MutinyMockSupport* mutiny_mock_scope(const char* name)

   Returns the named :c:struct:`MutinyMockSupport` scope.

The C mock API exposes :c:struct:`MutinyMockSupport` through
:c:func:`mutiny_mock`, which returns a pointer to a struct of function
pointers. This allows chaining calls in C99:

.. code-block:: c

   #include "mutiny/mock.h"

   /* expect */
   mutiny_mock()->expect_one_call("send")
       ->with_int_parameters("fd", 3)
       ->and_return_int_value(8);

   /* actual (in stub) */
   int n = (int)mutiny_mock()
       ->actual_call("send")
       ->with_int_parameters("fd", fd)
       ->int_return_value();

For named scopes: :c:func:`mutiny_mock_scope("name") <mutiny_mock_scope>` returns a pointer to
the named :c:struct:`MutinyMockSupport`.

Data store in C
~~~~~~~~~~~~~~~

.. code-block:: c

   mutiny_mock()->set_int_data("retval", 42);
   int v = mutiny_mock()->get_data("retval").value.int_value;

Custom comparators in C
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   static int my_equal(const void* a, const void* b) { return a == b; }
   static const char* my_to_string(const void* a) { (void)a; return "obj"; }

   mutiny_mock()->install_comparator("MyType", my_equal, my_to_string);

Complete C mock example:

.. literalinclude:: ../examples/tests/MockDocumentation.test.c
   :language: c

Examples
--------

.. list-table::
   :header-rows: 1

   * - Files
     - Demonstrates
   * - :source:`hello.test.c <examples/tests/hello.test.c>` +
       :source:`hello.test.cpp <examples/tests/hello.test.cpp>`
     - Two-file pattern: stubs a function pointer with ``MUTINY_PTR_SET``,
       checks output with ``CHECK_EQUAL_STRING``
   * - :source:`MockDocumentation.test.c <examples/tests/MockDocumentation.test.c>` +
       :source:`MockDocumentation.test.cpp <examples/tests/MockDocumentation.test.cpp>`
     - C mock interface: :c:func:`mutiny_mock`, typed parameters, custom
       comparator, data store
