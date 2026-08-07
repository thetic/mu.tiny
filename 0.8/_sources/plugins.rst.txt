Plugins
=======

Plugins let you run code before and after every test, parse custom
command-line flags, and create additional output formatters. *mu::tiny*
ships several built-in plugins; you can also write your own.

:cpp:class:`Plugin <mu::tiny::test::Plugin>` Base Class
---------------------------------------------------------

Subclass :cpp:class:`mu::tiny::test::Plugin` to intercept the test lifecycle.

.. code-block:: cpp

   // include/mu/tiny/test/Plugin.hpp
   class Plugin
   {
   public:
       Plugin(const String& name);

       virtual void pre_test_action(Shell&, Result&)  {}
       virtual void post_test_action(Shell&, Result&) {}

       virtual bool parse_arguments(int argc, const char* const* argv)
       { return false; }

       virtual String get_help() const { return ""; }
       virtual Output* create_output() { return nullptr; }
   };

Override the methods you need:

.. list-table::
   :header-rows: 1

   * - Method
     - When called
     - Typical use
   * - :cpp:func:`pre_test_action() <mu::tiny::test::Plugin::pre_test_action>`
     - Before each test body
     - Reset global state, start timers
   * - :cpp:func:`post_test_action() <mu::tiny::test::Plugin::post_test_action>`
     - After each test body
     - Restore pointers, verify mock expectations
   * - :cpp:func:`parse_arguments() <mu::tiny::test::Plugin::parse_arguments>`
     - During argument parsing
     - Inspect ``argv[0]``; activate plugin via a :option:`-p\<plugin\>` argument
   * - :cpp:func:`get_help() <mu::tiny::test::Plugin::get_help>`
     - When :option:`-h` is printed
     - Show plugin's flag description
   * - :cpp:func:`create_output() <mu::tiny::test::Plugin::create_output>`
     - At startup
     - Return a :cpp:class:`Output <mu::tiny::test::Output>` for an additional output format

Installing Plugins
------------------

Call :cpp:func:`CommandLineRunner::install_plugin() <mu::tiny::test::CommandLineRunner::install_plugin>`
in ``main()`` before :cpp:func:`run_all_tests() <mu::tiny::test::CommandLineRunner::run_all_tests>`:

.. code-block:: cpp

   #include "mu/tiny/test/CommandLineRunner.hpp"
   #include "MyPlugin.hpp"

   using mu::tiny::test::CommandLineRunner;

   int main(int argc, char** argv)
   {
       MyPlugin my_plugin;
       CommandLineRunner::install_plugin(my_plugin);
       return CommandLineRunner::run_all_tests(argc, argv);
   }

Plugins form a linked list; they run in installation order.

Writing a Custom Plugin
-----------------------

Example: a plugin that logs the name of every test that runs.

.. code-block:: cpp

   // LogPlugin.hpp
   #include "mu/tiny/test/Plugin.hpp"
   #include "mu/tiny/test/Shell.hpp"
   #include <cstdio>

   class LogPlugin : public mu::tiny::test::Plugin
   {
   public:
       LogPlugin() : mu::tiny::test::Plugin("LogPlugin") {}

       void pre_test_action(mu::tiny::test::Shell& test, mu::tiny::test::Result&) override
       {
           std::printf("[LOG] running %s::%s\n",
                       test.get_group(), test.get_name());
       }
   };

Use it in ``main()``:

.. code-block:: cpp

   LogPlugin log;
   CommandLineRunner::install_plugin(log);

Built-in Plugins
----------------

.. list-table::
   :header-rows: 1

   * - Plugin
     - Header
     - Purpose
     - Docs
   * - :cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>`
     - :ref:`mu/tiny/test/SetPointerPlugin.hpp <file_include_mu_tiny_test_SetPointerPlugin.hpp>`
     - Restore overwritten pointers after each test
     - :doc:`set-pointer-plugin`
   * - :cpp:class:`SupportPlugin <mu::tiny::mock::SupportPlugin>`
     - :ref:`mu/tiny/mock/SupportPlugin.hpp <file_include_mu_tiny_mock_SupportPlugin.hpp>`
     - Auto-verify and clear mock expectations
     - :doc:`mocking`
   * - :cpp:class:`JUnitOutputPlugin <mu::tiny::test::JUnitOutputPlugin>`
     - :ref:`mu/tiny/test/JUnitOutputPlugin.hpp <file_include_mu_tiny_test_JUnitOutputPlugin.hpp>`
     - Write JUnit XML output via :option:`-pjunit`
     - :doc:`junit-output`
   * - :cpp:class:`TapOutputPlugin <mu::tiny::test::TapOutputPlugin>`
     - :ref:`mu/tiny/test/TapOutputPlugin.hpp <file_include_mu_tiny_test_TapOutputPlugin.hpp>`
     - Write TAP version 13 output to stdout via :option:`-ptap`
     - :doc:`tap-output`

:cpp:class:`SetPointerPlugin <mu::tiny::test::SetPointerPlugin>` is automatically
created and available through the :c:macro:`MUTINY_PTR_SET` macro — you do not need to
install it manually if you only use the macro.

Examples
--------

Installing :cpp:class:`SupportPlugin <mu::tiny::mock::SupportPlugin>` with a custom
comparator alongside other plugins:

.. literalinclude:: ../examples/tests/main.cpp
   :language: cpp

.. toctree::
   :hidden:

   set-pointer-plugin
   junit-output
   tap-output
