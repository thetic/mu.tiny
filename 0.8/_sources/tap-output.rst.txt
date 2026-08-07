TAP Output
==========

*mu::tiny* can emit `Test Anything Protocol
<https://testanything.org/tap-version-13-specification.html>`_ (TAP) version 13
output via :cpp:class:`TapOutputPlugin <mu::tiny::test::TapOutputPlugin>`. TAP
is a simple line-based format that many CI systems, test harnesses, and tools
such as ``prove`` can consume directly.

Activating TAP Output
---------------------

Via command-line flag
~~~~~~~~~~~~~~~~~~~~~

Pass :option:`-ptap` when running your test executable:

.. code-block:: bash

   ./my_tests -ptap

TAP output is written to stdout. Normal console output is suppressed.

Via plugin in ``main()``
~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:class:`TapOutputPlugin <mu::tiny::test::TapOutputPlugin>` is installed
automatically when using
:cpp:func:`CommandLineRunner::run_all_tests <mu::tiny::test::CommandLineRunner::run_all_tests>`.
If you use a custom runner you can install it manually:

.. code-block:: cpp

   #include "mu/tiny/test/CommandLineRunner.hpp"
   #include "mu/tiny/test/Registry.hpp"
   #include "mu/tiny/test/TapOutputPlugin.hpp"

   int main(int argc, char** argv)
   {
       mu::tiny::test::TapOutputPlugin tap;
       mu::tiny::test::Registry::get_current_registry()->install_plugin(&tap);
       return mu::tiny::test::CommandLineRunner::run_all_tests(argc, argv);
   }

With the plugin installed, ``-ptap`` on the command line enables TAP output.
Without the flag, the console output is unchanged.

Output Format
-------------

The output follows `TAP version 13
<https://testanything.org/tap-version-13-specification.html>`_:

.. code-block:: text

   TAP version 13
   1..3
   ok 1 - Widget.ReturnsInitialValue
   not ok 2 - Widget.CanBeSet
     ---
     message: "widget.test.cpp:25: expected <42> but was <0>"
     severity: fail
     at:
       file: widget.test.cpp
       line: 25
     ...
   ok 3 - Widget.SkippedTest # SKIP SKIPPED_TEST

Key points:

- The version line ``TAP version 13`` always appears first.
- The plan line ``1..N`` follows immediately, stating the total test count.
- Each test line is ``ok`` or ``not ok``, numbered sequentially across all
  groups.
- Test identifiers use the form ``<Group>.<Test>``.
- Failing tests include a YAML diagnostic block (between ``---`` and ``...``)
  with ``message``, ``severity``, and ``at`` (file and line).
- ``severity`` is ``fail`` for assertion failures and ``comment`` for
  unexpected exceptions.
- Skipped (:c:macro:`SKIPPED_TEST`) and dynamically skipped
  (:c:macro:`SKIP_TEST`) tests use the ``# SKIP <reason>`` directive.

CI Integration
--------------

GitHub Actions
~~~~~~~~~~~~~~

Run the test binary with ``-ptap`` and pipe output to a TAP consumer. The
`tap-summary <https://github.com/nicolo-ribaudo/tap-summary>`_ action, or
Perl's built-in ``prove``, both work:

.. code-block:: yaml

   - name: Run tests (TAP)
     run: ./build/defaults/tests/my_tests -ptap | prove --stdin

.. code-block:: yaml

   - name: Run tests (TAP)
     run: ./build/defaults/tests/my_tests -ptap > results.tap

   - name: Validate TAP
     run: prove results.tap

Command line with ``prove``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   ./my_tests -ptap | prove --stdin
