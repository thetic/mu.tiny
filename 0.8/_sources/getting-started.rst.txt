Getting Started with mu::tiny
=============================

Building mu::tiny
-----------------

*mu::tiny* uses CMake. Clone the repository and configure:

.. code-block:: bash

   cmake -B build
   cmake --build build
   cmake --install build  # optional system install

Adding mu::tiny to Your Project
-------------------------------

The recommended approach uses CMake :cmake:module:`FetchContent <cmake:module:FetchContent>`
with :cmake:variable:`FIND_PACKAGE_ARGS <cmake:variable:FETCHCONTENT_TRY_FIND_PACKAGE_MODE>`.
This first attempts :cmake:command:`find_package <cmake:command:find_package>`; if *mu::tiny*
is not installed, CMake fetches and builds it from source automatically:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.24)
   project(my_tests)

   include(FetchContent)
   FetchContent_Declare(
       mu.tiny
       GIT_REPOSITORY https://github.com/thetic/mu.tiny.git
       GIT_TAG        v0.8.0
       FIND_PACKAGE_ARGS 0.8
   )
   FetchContent_MakeAvailable(mu.tiny)

   add_executable(my_tests main.cpp widget.test.cpp)
   target_link_libraries(my_tests PRIVATE mu::tiny)

   include(CTest)
   include(mu.tiny)
   mutiny_discover_tests(my_tests)

If *mu::tiny* is already installed and you prefer not to use
:cmake:module:`FetchContent <cmake:module:FetchContent>`, use
:cmake:command:`find_package <cmake:command:find_package>` directly:

.. code-block:: cmake

   find_package(mu.tiny 0.8 REQUIRED)

Headers
~~~~~~~

All public headers live under ``include/mu/tiny/``. The main headers you'll use:

.. list-table::
   :header-rows: 1

   * - Header
     - Purpose
   * - ``mu/tiny/test.hpp``
     - Test and assertion macros (:c:macro:`TEST_GROUP`, :c:macro:`TEST`, :c:macro:`CHECK`, etc.)
   * - ``mu/tiny/mock.hpp``
     - Mock framework (:cpp:func:`mock <mu::tiny::mock::mock>`, :cpp:class:`Support <mu::tiny::mock::Support>`)
   * - ``mu/tiny/test.h``
     - C interface (include in ``.test.c`` files)
   * - ``mu/tiny/test/CommandLineRunner.hpp``
     - :cpp:class:`CommandLineRunner <mu::tiny::test::CommandLineRunner>` (``main()`` runner)
   * - ``mu/tiny/test/Ordered.hpp``
     - :c:macro:`TEST_ORDERED` macro

Writing ``main()``
------------------

Every test executable needs a ``main()``. The simplest form uses
:cpp:class:`CommandLineRunner <mu::tiny::test::CommandLineRunner>`:

.. code-block:: cpp

   #include "mu/tiny/test/CommandLineRunner.hpp"

   using mu::tiny::test::CommandLineRunner;

   int main(int argc, char** argv)
   {
       return CommandLineRunner::run_all_tests(argc, argv);
   }

To add plugins (e.g.
:cpp:class:`JUnitOutputPlugin <mu::tiny::test::JUnitOutputPlugin>`,
:cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>`,
:cpp:class:`SupportPlugin <mu::tiny::mock::SupportPlugin>`), call
:cpp:func:`CommandLineRunner::install_plugin() <mu::tiny::test::CommandLineRunner::install_plugin>`
before :cpp:func:`run_all_tests() <mu::tiny::test::CommandLineRunner::run_all_tests>`:

.. code-block:: cpp

   #include "mu/tiny/test/CommandLineRunner.hpp"
   #include "mu/tiny/test/JUnitOutputPlugin.hpp"
   #include "mu/tiny/mock/SupportPlugin.hpp"

   using mu::tiny::test::CommandLineRunner;

   int main(int argc, char** argv)
   {
       mu::tiny::test::JUnitOutputPlugin junit;
       mu::tiny::mock::SupportPlugin mock_plugin;
       CommandLineRunner::install_plugin(junit);
       CommandLineRunner::install_plugin(mock_plugin);
       return CommandLineRunner::run_all_tests(argc, argv);
   }

Your First Test
---------------

.. literalinclude:: ../examples/tests/CheatSheet.test.cpp
   :language: cpp

- ``TEST_GROUP`` declares a group; the struct implicitly inherits from
  :cpp:class:`mu::tiny::test::Test`. ``setup()`` runs before each test body;
  ``teardown()`` runs after.
- ``TEST(group, name)`` defines a single test. The body is the
  ``{ ... }`` block that follows.
- A failing assertion immediately exits the test body (via ``longjmp``
  or exception).

Running Tests
-------------

Run the binary directly:

.. code-block:: bash

   ./build/tests/my_tests                  # run all
   ./build/tests/my_tests -v               # verbose: print each test name
   ./build/tests/my_tests -g CheatSheet    # only group "CheatSheet"
   ./build/tests/my_tests -n TestName      # only tests whose name contains this

Via CTest (after :cmake:command:`mutiny_discover_tests` in CMakeLists):

.. code-block:: bash

   ctest --preset GNU
   ctest --preset GNU -R CheatSheet

See :doc:`command-line-reference` for all flags.

Examples
--------

.. list-table::
   :header-rows: 1

   * - File
     - Demonstrates
   * - :source:`CheatSheet.test.cpp <examples/tests/CheatSheet.test.cpp>`
     - Minimal :c:macro:`TEST_GROUP` + :c:macro:`TEST` with :cpp:func:`setup() <mu::tiny::test::Test::setup>`, :c:macro:`MUTINY_PTR_SET`,
       and a handful of assertion macros
   * - :source:`CircularBuffer.test.cpp <examples/tests/CircularBuffer.test.cpp>`
     - Full group with setup/teardown, helper methods, and multiple assertion styles
