Ordered Tests
=============

Normally mutiny runs tests in an unspecified order (registration order,
possibly shuffled with :option:`-s [seed]`). ``OrderedTest`` lets you assign a
numeric level to selected tests so they run in level order, independent
of the registration order of all other tests.

Include ``"mutiny/test/Ordered.hpp"``.

``TEST_ORDERED(group, name, level)``
-------------------------------------

:c:macro:`TEST_ORDERED` declares a test that runs at the given level. Lower levels run first.
Tests at the same level run in registration order relative to each
other (``FirstEvent`` and ``SecondEvent`` both run at level 20 below).

.. literalinclude:: ../examples/tests/OrderedTest.test.cpp
   :language: cpp

The leading ``TEST(OrderedDemo, StartsUninitialized)`` is a regular test:
it runs before the ordered block. Ordered tests run as a block after all
unordered tests by default.

C Wrapper
---------

:c:macro:`TEST_ORDERED_C_WRAPPER(group, name, level) <TEST_ORDERED_C_WRAPPER>` wires a C test function
into an ordered test. Use it in the ``.test.cpp`` companion file
alongside ``TEST_C_WRAPPER``:

.. code-block:: cpp

   TEST_ORDERED_C_WRAPPER(Database, CreateSchema, 10)
   TEST_ORDERED_C_WRAPPER(Database, SeedData,     20)

Listing Ordered Tests
---------------------

Pass :option:`-lo` to print each ordered test's location as ``group.name.file.line``:

.. code-block:: console

   $ ./my_tests -lo
   OrderedDemo.Init.examples/tests/OrderedTest.test.cpp.53
   OrderedDemo.FirstEvent.examples/tests/OrderedTest.test.cpp.62
   OrderedDemo.SecondEvent.examples/tests/OrderedTest.test.cpp.68
   OrderedDemo.VerifyLog.examples/tests/OrderedTest.test.cpp.75
   OrderedDemo.Shutdown.examples/tests/OrderedTest.test.cpp.83

When to Use Ordered Tests
--------------------------

Ordered tests are appropriate when:

- You are integration-testing a stateful system (e.g. a database, a
  file system) where setup steps must precede query steps.
- The test itself mutates persistent state that subsequent tests depend
  on.

Avoid them when:

- Tests can be made independent through
  :cpp:func:`setup() <mu::tiny::test::Test::setup>`/:cpp:func:`teardown() <mu::tiny::test::Test::teardown>` —
  independent tests are more robust and can be run in isolation.
- You want the :option:`-s [seed]` (shuffle) flag to help detect ordering
  dependencies — ordered tests are excluded from the shuffle.

