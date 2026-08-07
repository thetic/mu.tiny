Assertion Macros
================

Include :ref:`"mu/tiny/test.hpp" <file_include_mu_tiny_test.hpp>` (C++) or
:ref:`"mu/tiny/test.h" <file_include_mu_tiny_test.h>` (C) for assertions.

A failing assertion immediately exits the current test body. Each macro
increments an internal check counter on success, which appears in the
summary.

Boolean
-------

.. list-table::
   :header-rows: 1

   * - Macro
     - Passes when
   * - :c:macro:`CHECK(condition) <CHECK>`
     - ``condition`` is truthy

.. code-block:: cpp

   CHECK(ptr != nullptr);
   CHECK(list.empty());
   CHECK(!error_occurred);

Equality
--------

``CHECK_EQUAL(expected, actual)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`CHECK_EQUAL` uses ``operator==`` and implicit arithmetic promotion to compare
``expected`` and ``actual``. Works with any type that supports
``operator==``. On failure, calls :cpp:func:`string_from() <mu::tiny::string_from>` to format the
message.

.. code-block:: cpp

   CHECK_EQUAL(42, compute_answer());
   CHECK_EQUAL(Status::OK, get_status());

To make :c:macro:`CHECK_EQUAL` work with your own types, provide:

1. ``operator==(const YourType&, const YourType&)``
2. :cpp:class:`mu::tiny::String` ``string_from(const YourType&)`` — used in the
   failure message

.. code-block:: cpp

   namespace mu { namespace tiny {
       String string_from(const Colour& c) { return c.name(); }
   } }

``CHECK_APPROX(expected, actual, threshold)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`CHECK_APPROX` checks that ``expected`` and ``actual`` differ by at
most ``threshold``. All three operands must share the same numeric type
(floating-point or integral).

.. code-block:: cpp

   CHECK_APPROX(3.14, compute_pi(), 0.001);
   CHECK_APPROX(1.0f, compute_float(), 0.01f);
   CHECK_APPROX(1000, compute_int(), 10);

Relational
----------

``CHECK_COMPARE(first, relop, second)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`CHECK_COMPARE` checks that ``first relop second`` is true. ``relop`` is a
literal operator like ``<``, ``>=``, ``!=``.

.. code-block:: cpp

   CHECK_COMPARE(result, >=, 0);
   CHECK_COMPARE(count, !=, 0);

String
------

.. list-table::
   :header-rows: 1

   * - Macro
     - Description
   * - :c:macro:`STRCMP_EQUAL(expected, actual) <STRCMP_EQUAL>`
     - ``strcmp`` equality for ``const char*``
   * - :c:macro:`STRNCMP_EQUAL(expected, actual, length) <STRNCMP_EQUAL>`
     - ``strncmp`` for first ``length`` chars
   * - :c:macro:`STRCMP_CONTAINS(expected, actual) <STRCMP_CONTAINS>`
     - checks that ``actual`` contains ``expected`` as a substring

.. code-block:: cpp

   STRCMP_EQUAL("hello", get_greeting());
   STRCMP_CONTAINS("error", log_output());

Memory
------

``MEMCMP_EQUAL(expected, actual, size)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`MEMCMP_EQUAL` compares ``size`` bytes starting at ``expected`` and
``actual`` using ``memcmp``. On failure, a hex dump is printed.

Enum
----

.. list-table::
   :header-rows: 1

   * - Macro
     - Description
   * - :c:macro:`ENUMS_EQUAL_INT(expected, actual) <ENUMS_EQUAL_INT>`
     - Compares two enum values as ``int``
   * - :c:macro:`ENUMS_EQUAL_TYPE(underlying_type, expected, actual) <ENUMS_EQUAL_TYPE>`
     - Compares with the specified underlying type

.. code-block:: cpp

   ENUMS_EQUAL_INT(State::IDLE, get_state());
   ENUMS_EQUAL_TYPE(uint8_t, Color::RED, get_color());

Failure and Exit
----------------

.. list-table::
   :header-rows: 1

   * - Macro
     - Description
   * - :c:macro:`FAIL_TEST(text) <FAIL_TEST>`
     - Unconditional failure with message.
   * - :c:macro:`PASS_TEST() <PASS_TEST>`
     - Exits the test immediately *without* marking it as failed — useful
       in assertion-faking helpers
   * - :c:macro:`SKIP_TEST(text) <SKIP_TEST>`
     - Marks the test as skipped and exits the body. The message appears in
       JUnit XML as ``<skipped message="..."/>``. Tests registered with
       :c:macro:`SKIPPED_TEST` are also reported as skipped.

.. code-block:: cpp

   if (unexpected_condition)
       FAIL_TEST("Should not reach this branch");

Exception
---------

``CHECK_THROWS(ExceptionType, expression)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`CHECK_THROWS` is only available when exceptions are enabled
(:c:macro:`MUTINY_HAVE_EXCEPTIONS`). Fails if ``expression`` does not throw
``ExceptionType``.

.. code-block:: cpp

   CHECK_THROWS(std::invalid_argument, parse_int("abc"));

``_TEXT`` Variants
------------------

Every assertion macro has a ``_TEXT`` variant that appends a custom
message to the failure output:

.. code-block:: cpp

   CHECK_EQUAL_TEXT(expected, actual, "iteration " + std::to_string(i));
   STRCMP_EQUAL_TEXT("ok", result, "after calling init()");

Examples
--------

.. list-table::
   :header-rows: 1

   * - File
     - Demonstrates
   * - :source:`CheatSheet.test.cpp <examples/tests/CheatSheet.test.cpp>`
     - :c:macro:`CHECK_EQUAL`, :c:macro:`CHECK`, :c:macro:`STRCMP_EQUAL` in a minimal test
   * - :source:`CircularBuffer.test.cpp <examples/tests/CircularBuffer.test.cpp>`
     - :c:macro:`CHECK`, :c:macro:`CHECK_EQUAL`, :c:macro:`STRCMP_EQUAL` across a variety of test scenarios
