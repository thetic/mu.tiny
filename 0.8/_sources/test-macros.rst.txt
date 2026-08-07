Test Definition Macros
======================

Include :ref:`mu/tiny/test.hpp <file_include_mu_tiny_test.hpp>` for all macros on this page.

Test Group and Lifecycle
------------------------

``TEST_GROUP(group)``
~~~~~~~~~~~~~~~~~~~~~

:c:macro:`TEST_GROUP` declares a test group. Expands to a struct definition; add
:cpp:func:`setup() <mu::tiny::test::Test::setup>` and
:cpp:func:`teardown() <mu::tiny::test::Test::teardown>` inside it.

.. code-block:: cpp

   TEST_GROUP(MyGroup)
   {
       MyObject* obj;

       void setup() override    { obj = new MyObject; }
       void teardown() override { delete obj; }
   };

All tests in the group share a fresh instance of the struct (constructed
before :cpp:func:`setup() <mu::tiny::test::Test::setup>`, destroyed after
:cpp:func:`teardown() <mu::tiny::test::Test::teardown>`).

``TEST(group, name)``
~~~~~~~~~~~~~~~~~~~~~

:c:macro:`TEST` defines a test belonging to ``group``. The ``{ ... }`` block that
follows is the test body.

.. code-block:: cpp

   TEST(MyGroup, DoesTheThing)
   {
       CHECK(obj->do_thing());
   }

Skipping and Expected Failures
-------------------------------

``SKIPPED_TEST(group, name)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`SKIPPED_TEST` marks a test as skipped. It is registered but skipped during
normal runs. Use :option:`-rs` to run skipped tests anyway.

.. code-block:: cpp

   SKIPPED_TEST(MyGroup, NotImplementedYet)
   {
       // will not run unless -rs is passed
       CHECK(false);
   }

Skipped tests appear in the summary count as "skipped".

``XFAIL_TEST(group, name)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`XFAIL_TEST` declares a test that is expected to fail. The test runner
treats a failure as a pass and a pass as a failure. Useful for
documenting known bugs that cannot yet be fixed.

.. code-block:: cpp

   XFAIL_TEST(MyGroup, KnownBug)
   {
       CHECK_EQUAL(expected, buggy_function()); // currently produces wrong result
   }

Metadata
--------

``TEST_PROPERTY(name, value)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`TEST_PROPERTY` attaches a key/value metadata pair to the currently running
test. Emitted in :doc:`JUnit XML output <junit-output>` as ``<property>`` elements. Useful for
CI tagging (e.g. requirement IDs).

.. code-block:: cpp

   TEST(MyGroup, Requirement42)
   {
       TEST_PROPERTY("requirement", "REQ-42");
       CHECK(meets_requirement_42());
   }

Ordered Tests
-------------

Normally *mu::tiny* runs tests in an unspecified order (registration order,
possibly shuffled with :option:`-s [seed]`).
:cpp:class:`OrderedShell <mu::tiny::test::OrderedShell>` lets you assign a
numeric level to selected tests so they run in level order, independent of the
registration order of all other tests.

``TEST_ORDERED(group, name, level)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:c:macro:`TEST_ORDERED` declares a test that runs at the given level. Lower
levels run first. Tests at the same level run in registration order relative to
each other (``FirstEvent`` and ``SecondEvent`` both run at level 20 below).

.. literalinclude:: ../examples/tests/OrderedTest.test.cpp
   :language: cpp

The leading ``TEST(OrderedDemo, StartsUninitialized)`` is a regular test: it
runs before the ordered block. Ordered tests run as a block after all unordered
tests by default.

Listing Ordered Tests
~~~~~~~~~~~~~~~~~~~~~

Pass :option:`-lo` to print each ordered test's location as
``group.name.file.line``:

.. code-block:: console

   $ ./my_tests -lo
   OrderedDemo.Init.examples/tests/OrderedTest.test.cpp.53
   OrderedDemo.FirstEvent.examples/tests/OrderedTest.test.cpp.62
   OrderedDemo.SecondEvent.examples/tests/OrderedTest.test.cpp.68
   OrderedDemo.VerifyLog.examples/tests/OrderedTest.test.cpp.75
   OrderedDemo.Shutdown.examples/tests/OrderedTest.test.cpp.83

When to Use Ordered Tests
~~~~~~~~~~~~~~~~~~~~~~~~~

Ordered tests are appropriate when:

- You are integration-testing a stateful system (e.g. a database, a file
  system) where setup steps must precede query steps.
- The test itself mutates persistent state that subsequent tests depend on.

Avoid them when:

- Tests can be made independent through
  :cpp:func:`setup() <mu::tiny::test::Test::setup>`/:cpp:func:`teardown() <mu::tiny::test::Test::teardown>` —
  independent tests are more robust and can be run in isolation.
- You want the :option:`-s [seed]` (shuffle) flag to help detect ordering
  dependencies — ordered tests are excluded from the shuffle.

C Wrapper Macros
----------------

These bridge C test files into the C++ test runner. See
:doc:`testing-in-c` for full details.

.. list-table::
   :header-rows: 1

   * - Macro
     - Purpose
   * - :c:macro:`TEST_GROUP_C_WRAPPER(group) <TEST_GROUP_C_WRAPPER>`
     - Declares a C++ :c:macro:`TEST_GROUP` that calls C setup/teardown
   * - :c:macro:`TEST_GROUP_C_SETUP_WRAPPER(group) <TEST_GROUP_C_SETUP_WRAPPER>`
     - Wires C setup into the C++ group's :cpp:func:`setup() <mu::tiny::test::Test::setup>`
   * - :c:macro:`TEST_GROUP_C_TEARDOWN_WRAPPER(group) <TEST_GROUP_C_TEARDOWN_WRAPPER>`
     - Wires C teardown into the C++ group's :cpp:func:`teardown() <mu::tiny::test::Test::teardown>`
   * - :c:macro:`TEST_C_WRAPPER(group, name) <TEST_C_WRAPPER>`
     - Wires a C test function into a C++ :c:macro:`TEST`
   * - :c:macro:`SKIPPED_TEST_C_WRAPPER(group, name) <SKIPPED_TEST_C_WRAPPER>`
     - Same as above but skipped
   * - :c:macro:`XFAIL_TEST_C_WRAPPER(group, name) <XFAIL_TEST_C_WRAPPER>`
     - Same as above but expected to fail
   * - :c:macro:`TEST_ORDERED_C_WRAPPER(group, name, level) <TEST_ORDERED_C_WRAPPER>`
     - Wires a C test function into a :c:macro:`TEST_ORDERED`

Examples
--------

.. list-table::
   :header-rows: 1

   * - File
     - Demonstrates
   * - :source:`CheatSheet.test.cpp <examples/tests/CheatSheet.test.cpp>`
     - :c:macro:`TEST_GROUP`, :c:macro:`TEST`, :cpp:func:`setup() <mu::tiny::test::Test::setup>`, :cpp:func:`teardown() <mu::tiny::test::Test::teardown>`, and :c:macro:`MUTINY_PTR_SET`
   * - :source:`hello.test.c <examples/tests/hello.test.c>` +
       :source:`hello.test.cpp <examples/tests/hello.test.cpp>`
     - C-wrapper two-file pattern with :c:macro:`TEST_GROUP_SETUP`, :c:macro:`TEST`, and :c:macro:`TEST_C_WRAPPER`
   * - :source:`OrderedTest.test.cpp <examples/tests/OrderedTest.test.cpp>`
     - :c:macro:`TEST_ORDERED` alongside regular :c:macro:`TEST`
