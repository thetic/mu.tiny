SetPointerPlugin
================

:cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>` solves a common
embedded-testing problem: swapping out a function pointer (or any global pointer)
for a test stub, then automatically restoring it after the test.

Include :ref:`"mu/tiny/test.hpp" <file_include_mu_tiny_test.hpp>` — :cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>` is included
transitively, and the :c:macro:`MUTINY_PTR_SET` macro is available without any
additional install step.

The Macro
---------

.. code-block:: cpp

   MUTINY_PTR_SET(pointer, replacement)

- Saves the current value of ``pointer``.
- Replaces ``pointer`` with ``replacement``.
- After the test body (in :cpp:func:`post_test_action() <mu::tiny::test::Plugin::post_test_action>`), restores ``pointer``
  to the saved value.

Both ``pointer`` and ``replacement`` must be the same pointer-to-T type.
The macro is type-safe: it uses a template that takes ``T*& a, T* b``.

Example: Stubbing a Function Pointer
--------------------------------------

.. code-block:: c

   /* module.h */
   extern int (*platform_read)(int fd, void* buf, size_t n);

.. code-block:: cpp

   /* module.test.cpp */
   #include "mu/tiny/test.hpp"
   #include "module.h"

   static int stub_read(int, void* buf, size_t n)
   {
       memset(buf, 0xAB, n);
       return (int)n;
   }

   TEST_GROUP(ModuleRead) {};

   TEST(ModuleRead, UsesStub)
   {
       MUTINY_PTR_SET(platform_read, stub_read);
       // platform_read now points to stub_read
       // after this test, platform_read is restored automatically
       CHECK_EQUAL(4, read_four_bytes());
   }

Example: Replacing a Global Object Pointer
-------------------------------------------

.. code-block:: cpp

   static Logger* fake_logger = new FakeLogger;

   TEST(MyGroup, LogsOnError)
   {
       MUTINY_PTR_SET(g_logger, fake_logger);
       trigger_error();
       CHECK(fake_logger->was_called());
   }

Use Cases
---------

- **Function pointer stubs**: swap platform I/O, timers, or OS calls.
- **Global state**: temporarily replace a singleton or global object.
- **Contract checking**: swap in an assertion-catching fake to test error
  paths.

Limit
-----

:cpp:member:`SetPointerPlugin::max_set <mu::tiny::test::SetPointerPlugin::max_set>` = 32 — at most 32 pointers can be saved
simultaneously per test. Each :c:macro:`MUTINY_PTR_SET` call consumes one slot;
the slots are released at the end of the test.

Exceeding 32 calls in a single test body will crash or corrupt memory.
If you need more than 32, consider restructuring your test.

How It Works
------------

:cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>` is a
:cpp:class:`Plugin <mu::tiny::test::Plugin>` with a
:cpp:func:`post_test_action() <mu::tiny::test::Plugin::post_test_action>`
that iterates the saved location/value pairs and writes each original
value back. The static storage array means no heap allocation is needed,
which is safe even in environments without ``malloc``.

Examples
--------

.. list-table::
   :header-rows: 1

   * - File
     - Demonstrates
   * - :source:`CheatSheet.test.cpp <examples/tests/CheatSheet.test.cpp>`
     - :c:macro:`MUTINY_PTR_SET` swapping a function pointer in :cpp:func:`setup() <mu::tiny::test::Test::setup>`
   * - :source:`hello.test.c <examples/tests/hello.test.c>` +
       :source:`hello.test.cpp <examples/tests/hello.test.cpp>`
     - Function pointer stub pattern in C (manual save/restore instead of
       the macro, showing the underlying technique)
