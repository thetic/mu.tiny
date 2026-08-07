JUnit XML Output
================

*mu::tiny* can emit JUnit-compatible XML files via :cpp:class:`JUnitOutputPlugin <mu::tiny::test::JUnitOutputPlugin>`. CI
systems such as Jenkins, GitHub Actions, and GitLab CI can parse this
format to display test results in their dashboards.

Activating JUnit Output
-----------------------

Via command-line flag
~~~~~~~~~~~~~~~~~~~~~

Pass :option:`-pjunit` when running your test executable:

.. code-block:: bash

   ./my_tests -pjunit

The output file is named after the executable: running ``./my_tests -pjunit``
writes ``my_tests.xml``.

To choose a different name, use :option:`-pjunit=\<name\>`:

.. code-block:: bash

   ./my_tests -pjunit=widget_suite

This writes ``widget_suite.xml``.

Via plugin in ``main()``
~~~~~~~~~~~~~~~~~~~~~~~~~

Install the plugin via :cpp:class:`Registry <mu::tiny::test::Registry>` before calling
:cpp:func:`run_all_tests <mu::tiny::test::CommandLineRunner::run_all_tests>`:

.. code-block:: cpp

   #include "mu/tiny/test/CommandLineRunner.hpp"
   #include "mu/tiny/test/JUnitOutputPlugin.hpp"
   #include "mu/tiny/test/Registry.hpp"

   int main(int argc, char** argv)
   {
       // Pass the executable basename so that bare -pjunit writes
       // <basename>.xml rather than mutiny.xml.
       mu::tiny::test::JUnitOutputPlugin junit("my_suite");
       mu::tiny::test::Registry::get_current_registry()->install_plugin(&junit);
       return mu::tiny::test::CommandLineRunner::run_all_tests(argc, argv);
   }

The string passed to the constructor is used as the package name when
``-pjunit`` is given without an explicit ``=<name>`` suffix. It can be
overridden at runtime with ``-pjunit=<name>``. Passing an empty string
(the default) makes bare ``-pjunit`` write to ``mutiny.xml`` with no
package prefix in classnames.

With the plugin installed, ``-pjunit`` on the command line enables the
XML output. Without the flag, no XML is written and the console output
is unchanged.

Output File Location
--------------------

JUnit XML is written to the current working directory.

When running the executable directly, a single file is produced for the
entire run.  The file is named ``<name>.xml``, where ``<name>`` comes from:

- the ``-pjunit=<name>`` argument if given, or
- the default package name supplied to the :cpp:class:`JUnitOutputPlugin
  <mu::tiny::test::JUnitOutputPlugin>` constructor (the executable basename
  when going through :cpp:func:`CommandLineRunner::run_all_tests
  <mu::tiny::test::CommandLineRunner::run_all_tests>`), or
- ``mutiny`` if no default was provided.

When using :cmake:variable:`MUTINY_JUNIT_REPORT` with :cmake:command:`mutiny_discover_tests`,
CTest runs each test group as a separate process and gives each one a unique
``-pjunit=<target>.<group>`` argument, producing files such as
``my_tests.Widget.xml`` and ``my_tests.Engine.xml``.  This avoids collisions
when CTest runs tests in parallel.

When :cmake:variable:`MUTINY_TESTS_DETAILED` is also enabled, non-ordered
tests run individually and each receives ``-pjunit=<target>.<group>.<test>``,
so every test case gets its own file (e.g. ``my_tests.Widget.CanBeSet.xml``).
Ordered groups are still run as a unit and share one file per group.

XML Format
----------

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8" ?>
   <testsuites tests="3" failures="1" errors="0" skipped="0"
               time="0.001" timestamp="2024-01-15T10:30:00">
     <testsuite errors="0" failures="1" skipped="0" assertions="5"
                name="Widget" tests="3" time="0.001"
                timestamp="2024-01-15T10:30:00">
       <testcase classname="my_tests.Widget" name="ReturnsInitialValue"
                 assertions="2" time="0.000" file="widget.test.cpp" line="12">
       </testcase>
       <testcase classname="my_tests.Widget" name="CanBeSet"
                 assertions="1" time="0.000" file="widget.test.cpp" line="20">
         <failure message="widget.test.cpp:25: expected &lt;42&gt; but was &lt;0&gt;"
                  type="AssertionFailedError">
           widget.test.cpp:25: expected &lt;42&gt; but was &lt;0&gt;
         </failure>
       </testcase>
       <testcase classname="my_tests.Widget" name="SkippedTest"
                 assertions="0" time="0.000" file="widget.test.cpp" line="30">
         <skipped />
       </testcase>
     </testsuite>
   </testsuites>

Key points:

- The top-level ``<testsuites>`` element carries totals across all groups.
- Each group produces one ``<testsuite>`` child.
- ``classname`` is ``<package>.<group>`` where ``<package>`` is the name
  passed to ``-pjunit``.
- ``file`` and ``line`` attributes locate each test in the source tree.
- :c:macro:`TEST_PROPERTY` annotations appear as ``<property>`` elements
  inside a ``<properties>`` block within ``<testcase>``.

CI Integration
--------------

GitHub Actions
~~~~~~~~~~~~~~

.. code-block:: yaml

   - name: Run tests
     run: cmake --build --preset defaults && ctest --preset defaults
     env:
       MUTINY_JUNIT_REPORT: ON

   - name: Publish test results
     uses: EnricoMi/publish-unit-test-result-action@v2
     with:
       files: build/**/*.xml

GitLab CI
~~~~~~~~~

.. code-block:: yaml

   test:
     script:
       - cmake --build --preset defaults
       - ctest --preset defaults
     artifacts:
       reports:
         junit: build/**/*.xml

Jenkins
~~~~~~~

Configure the "Publish JUnit test result report" post-build step with
the pattern ``build/**/*.xml``.
