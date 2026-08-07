Command-Line Reference
======================

All flags are passed directly to the test executable:

.. code-block:: bash

   ./my_tests [flags...]

Run ``./my_tests -h`` to print the built-in help screen (also lists any
plugin-provided flags).

Query Flags (do not run tests)
-------------------------------

.. option:: -h

   Print help screen and exit.

.. option:: -lg

   Print all group names, space-separated.

.. option:: -ln

   Print all test names as ``group.name``, space-separated.

.. option:: -ll

   Print test locations as ``group.name.file.line``.

.. option:: -lo

   Print ordered test locations as ``group.name.file.line``.

.. option:: -lgl

   Print group source locations as ``group.file.line``.  Each entry points to
   the :c:macro:`TEST_GROUP` declaration rather than an individual test.

Output Flags
------------

.. option:: -c

   Colorize output: green for pass, red for fail.

.. option:: -v

   Verbose: print each test name as it runs.

.. option:: -vv

   Very verbose: print internal framework state.

Plugin Flags
------------

Plugin flags begin with ``-p``. The plugin must be installed in
``main()`` to parse them.

.. option:: -p<plugin>

   Invoke the named plugin flag.  Each plugin registered in ``main()``
   receives every ``-p``-prefixed argument and may act on flags it
   recognises.

.. option:: -pjunit

   Write JUnit XML output files (requires :cpp:class:`JUnitOutputPlugin <mu::tiny::test::JUnitOutputPlugin>`).
   See :doc:`junit-output`.

.. option:: -ptap

   Write TAP version 13 output to stdout (requires :cpp:class:`TapOutputPlugin <mu::tiny::test::TapOutputPlugin>`).
   See :doc:`tap-output`.

Filter Flags
------------

Multiple filter flags can be combined; all must match for a test to run.

Include filters (substring match by default)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. option:: -g <group>

   Run tests whose group name *contains* ``<group>``.

.. option:: -n <name>

   Run tests whose test name *contains* ``<name>``.

.. option:: -t <group>.<name>

   Run tests whose group contains ``<group>`` **and** name contains ``<name>``.

Include filters (exact match)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. option:: -sg <group>

   Run tests whose group *exactly equals* ``<group>``.

.. option:: -sn <name>

   Run tests whose test name *exactly equals* ``<name>``.

.. option:: -st <group>.<name>

   Run tests where both group and name exactly equal the given values.

Exclude filters (substring match)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. option:: -xg <group>

   Exclude tests whose group contains ``<group>``.

.. option:: -xn <name>

   Exclude tests whose name contains ``<name>``.

.. option:: -xt <group>.<name>

   Exclude tests whose group and name both contain the respective substrings.

Exclude filters (exact match)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. option:: -xsg <group>

   Exclude tests whose group exactly equals ``<group>``.

.. option:: -xsn <name>

   Exclude tests whose name exactly equals ``<name>``.

.. option:: -xst <group>.<name>

   Exclude tests where both group and name exactly equal the given values.

Copy-paste filter
~~~~~~~~~~~~~~~~~

Verbose output (:option:`-v`) prints lines like ``TEST(Widget, CanBeSet)``.
You can paste this directly on the command line to run only that test:

.. code-block:: bash

   ./my_tests "TEST(Widget, CanBeSet)"
   ./my_tests "SKIPPED_TEST(Widget, Skipped)"

This performs an exact match on both group and name.

Execution Flags
---------------

.. option:: -b

   Run tests in reverse registration order.

.. option:: -s [seed]

   Shuffle test execution order randomly. Optional ``seed`` (integer > 0)
   makes the order reproducible; if omitted, seed is time-based and
   printed to console.

.. option:: -r[#]

   Repeat the full test suite ``#`` times. If ``#`` is omitted, repeats
   twice. Useful for detecting order-dependent failures.

.. option:: -rs

   Run skipped tests (:c:macro:`SKIPPED_TEST`) as if they were normal tests.

.. option:: -f

   Crash on first failure instead of continuing (useful with a debugger).

.. option:: -e

   Do not rethrow unexpected exceptions as failures (catch and continue).
