Mocking
=======

Include :ref:`mu/tiny/mock.hpp <file_include_mu_tiny_mock.hpp>` for the C++ mock API.

Accessing the Mock Object
--------------------------

The central class is :cpp:class:`mu::tiny::mock::Support`, accessed via the
:cpp:func:`mu::tiny::mock::mock` free function:

.. code-block:: cpp

   mu::tiny::mock::mock()           // global mock scope
   mu::tiny::mock::mock("network")  // named scope — keeps expectations separate

Named scopes are independent: ``mock("db")`` and ``mock("net")`` have
separate expectation lists.

Basic Workflow
--------------

Every test that uses mocks should follow this pattern:

.. code-block:: cpp

   TEST(MyGroup, CallsNetworkSend)
   {
       // 1. Set expectations
       mock("net").expect_one_call("send")
                  .with_parameter("data", buffer)
                  .and_return_value(42);

       // 2. Exercise the code under test
       int result = subject.send_data(buffer);

       // 3. Verify
       CHECK_EQUAL(42, result);
       mock().check_expectations(); // fails if expected calls were not made
       mock().clear();              // reset for next test
   }

:cpp:func:`check_expectations() <mu::tiny::mock::Support::check_expectations>` +
:cpp:func:`clear() <mu::tiny::mock::Support::clear>` in teardown is cleaner than
per-test calls. :cpp:class:`SupportPlugin <mu::tiny::mock::SupportPlugin>` does this
automatically — see :doc:`plugins`.

Expecting Calls
---------------

.. list-table::
   :header-rows: 1

   * - Method
     - Description
   * - ``mock().``\ :cpp:func:`expect_one_call("func") <mu::tiny::mock::Support::expect_one_call>`
     - Expect exactly one call to ``"func"``
   * - ``mock().``\ :cpp:func:`expect_n_calls(n, "func") <mu::tiny::mock::Support::expect_n_calls>`
     - Expect exactly ``n`` calls
   * - ``mock().``\ :cpp:func:`expect_no_call("func") <mu::tiny::mock::Support::expect_no_call>`
     - Expect that ``"func"`` is never called

Parameter Matching
------------------

Chain :cpp:func:`.with_parameter(name, value) <mu::tiny::mock::ExpectedCall::with_parameter>` on an expected call to constrain
parameter values; use :cpp:func:`.with_parameter() <mu::tiny::mock::ActualCall::with_parameter>` on the
actual call side with the same overloads. Both accept: ``bool``,
``int``, ``unsigned int``, ``long``, ``unsigned long``, ``long long``,
``unsigned long long``, ``double``, ``const char*``, ``void*``,
``const void*``, ``void(*)()``, and memory buffers
``(const unsigned char*, size_t)``. Fixed-width types from
``<stdint.h>`` (``size_t``, ``uint32_t``, ``int64_t``, etc.) work
directly because they are typedefs for these fundamental types.

.. code-block:: cpp

   mock().expect_one_call("write")
         .with_parameter("fd", 1)
         .with_parameter("buf", data_ptr)
         .with_parameter("count", (size_t)8);

For ``double`` with a tolerance:

.. code-block:: cpp

   mock().expect_one_call("scale")
         .with_parameter("factor", 1.0, 0.001); // value ± tolerance

Custom Types
~~~~~~~~~~~~

Use :cpp:func:`with_parameter_of_type() <mu::tiny::mock::ExpectedCall::with_parameter_of_type>` on the
expected call and :cpp:func:`with_parameter_of_type() <mu::tiny::mock::ActualCall::with_parameter_of_type>`
on the actual call side for non-native types. A comparator must be installed first — see
`Custom Comparators`_ below.

.. code-block:: cpp

   mock().expect_one_call("process")
         .with_parameter_of_type("Packet", "pkt", &expected_packet);

Relaxing Parameter Checks
~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:func:`.ignore_other_parameters() <mu::tiny::mock::ExpectedCall::ignore_other_parameters>` on an expected call: any extra parameters
the actual call provides beyond the matched set are ignored.

:cpp:func:`mock().ignore_other_calls() <mu::tiny::mock::Support::ignore_other_calls>` on the mock support: silently accept any
call that has no matching expectation.

Output Parameters
-----------------

Sometimes the mock needs to write data back to the caller through a
pointer parameter.

On the expected side:

.. code-block:: cpp

   mock().expect_one_call("read")
         .with_output_parameter_returning("buf", &source_data, sizeof(source_data));

On the actual call side (in your mock implementation):

.. code-block:: cpp

   mock().actual_call("read")
         .with_output_parameter("buf", output_buffer);

The framework copies ``source_data`` into ``output_buffer`` when the
call is matched.

For custom types use
:cpp:func:`with_output_parameter_of_type_returning() <mu::tiny::mock::ExpectedCall::with_output_parameter_of_type_returning>` +
:cpp:func:`with_output_parameter_of_type() <mu::tiny::mock::ActualCall::with_output_parameter_of_type>`
with a registered copier.

Return Values
-------------

Set the return value on the expected call with
:cpp:func:`and_return_value() <mu::tiny::mock::ExpectedCall::and_return_value>`. The overload accepts all basic types plus
``void*``, ``const void*``, and ``void(*)()``.

Retrieve the return value in your mock stub with the template accessor
``return_value<T>()``:

.. code-block:: cpp

   // In the mock stub:
   return mock().actual_call("compute")
                .with_parameter("x", x)
                .return_value<int>();

   // Or with a default if no expectation was set:
   return mock().actual_call("compute")
                .with_parameter("x", x)
                .return_value_or_default<int>(0);

``return_value<T>()`` and ``return_value_or_default<T>()`` are available
on both :cpp:class:`ActualCall <mu::tiny::mock::ActualCall>` and
:cpp:class:`Support <mu::tiny::mock::Support>`. They work with any type
that has a ``NamedValue::get_value<T>()`` specialization — all fundamental
types plus ``const char*``, ``void*``, ``const void*``, ``void(*)()``,
and any ``<stdint.h>`` typedef (``uint32_t``, ``int64_t``, etc.).


Object Binding
--------------

Use :cpp:func:`.on_object() <mu::tiny::mock::ExpectedCall::on_object>` to scope an expectation to a specific object
instance. Useful when multiple objects of the same type are in play.

.. code-block:: cpp

   Widget w1, w2;
   mock().expect_one_call("tick").on_object(&w1);
   // only a call on &w1 satisfies this expectation

In the mock stub:

.. code-block:: cpp

   mock().actual_call("tick").on_object(this);

Call Ordering
-------------

By default calls can occur in any order. To enforce order:

.. code-block:: cpp

   mock().strict_order();
   mock().expect_one_call("first") .with_call_order(1);
   mock().expect_one_call("second").with_call_order(2);
   mock().expect_one_call("third") .with_call_order(3);

For a range of acceptable positions: :cpp:func:`.with_call_order() <mu::tiny::mock::ExpectedCall::with_call_order>`.

Enable / Disable / Tracing
--------------------------

.. code-block:: cpp

   mock().disable();  // all actual_call() calls are silently accepted
   mock().enable();   // back to normal checking

   mock().tracing(true);       // print each actual call as it happens
   mock().crash_on_failure();  // crash (useful with a debugger) instead of reporting failure

Data Store
----------

The mock object doubles as a key/value store. Pass data between the test
and stub code without extra globals:

.. code-block:: cpp

   // In test setup:
   mock().set_data("timeout_ms", 100);

   // In mock stub:
   int timeout = mock().get_data("timeout_ms").get_value<int>();

``get_value<T>()`` works with fixed-width types too:

.. code-block:: cpp

   mock().set_data("calibration", static_cast<int64_t>(42));
   auto cal = mock().get_data("calibration").get_value<int64_t>();

:cpp:func:`set_data() <mu::tiny::mock::Support::set_data>` is overloaded for: ``bool``, ``int``, ``unsigned int``,
``long``, ``unsigned long``, ``long long``, ``unsigned long long``,
``const char*``, ``double``, ``void*``, ``const void*``, ``void(*)()``.
For object types:
:cpp:func:`set_data_object() <mu::tiny::mock::Support::set_data_object>` /
:cpp:func:`set_data_const_object() <mu::tiny::mock::Support::set_data_const_object>`.

Custom Comparators
------------------

Install a :cpp:class:`NamedValueComparator <mu::tiny::mock::NamedValueComparator>` to make
:cpp:func:`with_parameter_of_type() <mu::tiny::mock::ExpectedCall::with_parameter_of_type>` work for your type:

Template comparator (simplest)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:class:`TypedMockComparator\<T\> <mu::tiny::mock::TypedMockComparator>` requires
``operator==`` and :cpp:func:`string_from() <mu::tiny::string_from>`:

.. code-block:: cpp

   TypedMockComparator<Packet> packet_cmp;
   mock().install_comparator("Packet", packet_cmp);
   // Requires: operator==(const Packet&, const Packet&)
   // and: mu::tiny::String string_from(const Packet&)

Function-based comparator
~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:class:`FunctionComparator <mu::tiny::mock::FunctionComparator>` wraps plain function
pointers instead of a subclass:

.. code-block:: cpp

   FunctionComparator packet_cmp(
       [](const void* a, const void* b) {
           return *static_cast<const Packet*>(a) == *static_cast<const Packet*>(b);
       },
       [](const void* a) -> mu::tiny::String {
           return static_cast<const Packet*>(a)->to_string();
       }
   );
   mock().install_comparator("Packet", packet_cmp);

Subclass comparator
~~~~~~~~~~~~~~~~~~~

Subclass :cpp:class:`NamedValueComparator <mu::tiny::mock::NamedValueComparator>` directly
for full control:

.. code-block:: cpp

   class PacketComparator : public mu::tiny::mock::NamedValueComparator
   {
   public:
       bool is_equal(const void* a, const void* b) override {
           return *static_cast<const Packet*>(a) == *static_cast<const Packet*>(b);
       }
       mu::tiny::String value_to_string(const void* a) override {
           return static_cast<const Packet*>(a)->to_string();
       }
   };

Remove all installed comparators and copiers with
:cpp:func:`mock().remove_all_comparators_and_copiers() <mu::tiny::mock::Support::remove_all_comparators_and_copiers>`.

Custom Copiers
--------------

A :cpp:class:`NamedValueCopier <mu::tiny::mock::NamedValueCopier>` is needed when using
output parameters of custom types:

:cpp:class:`TypedMockCopier\<T\> <mu::tiny::mock::TypedMockCopier>` uses ``operator=``:

.. code-block:: cpp

   TypedMockCopier<Packet> packet_copier;
   mock().install_copier("Packet", packet_copier);
   // Requires: Packet::operator=

:cpp:class:`FunctionCopier <mu::tiny::mock::FunctionCopier>` wraps a plain copy function:

.. code-block:: cpp

   FunctionCopier packet_copier([](void* dst, const void* src) {
       *static_cast<Packet*>(dst) = *static_cast<const Packet*>(src);
   });
   mock().install_copier("Packet", packet_copier);

SupportPlugin
-------------

:cpp:class:`SupportPlugin <mu::tiny::mock::SupportPlugin>` automatically calls
:cpp:func:`check_expectations() <mu::tiny::mock::Support::check_expectations>` and
:cpp:func:`clear() <mu::tiny::mock::Support::clear>` after every test, and manages
comparator/copier lifetime:

.. code-block:: cpp

   // main.cpp
   mu::tiny::mock::SupportPlugin mock_plugin;
   mock_plugin.install_comparator("Packet", packet_cmp);
   reg->install_plugin(&mock_plugin);

With the plugin installed, tests no longer need explicit
:cpp:func:`check_expectations() <mu::tiny::mock::Support::check_expectations>`/:cpp:func:`clear() <mu::tiny::mock::Support::clear>` calls. See :doc:`plugins`.

Examples
--------

Core mock workflow —
:cpp:func:`expect_one_call() <mu::tiny::mock::Support::expect_one_call>`,
:cpp:func:`expect_n_calls() <mu::tiny::mock::Support::expect_n_calls>`,
:cpp:func:`with_parameter() <mu::tiny::mock::ExpectedCall::with_parameter>`,
:cpp:func:`and_return_value() <mu::tiny::mock::ExpectedCall::and_return_value>`,
:cpp:func:`ignore_other_parameters() <mu::tiny::mock::ExpectedCall::ignore_other_parameters>`,
:cpp:func:`check_expectations() <mu::tiny::mock::Support::check_expectations>`,
:cpp:func:`clear() <mu::tiny::mock::Support::clear>`:

.. literalinclude:: ../examples/tests/MockCheatSheet.test.cpp
   :language: cpp

Further examples:

- :source:`MockDocumentation.test.cpp <examples/tests/MockDocumentation.test.cpp>`
  — :cpp:func:`on_object() <mu::tiny::mock::ExpectedCall::on_object>`,
  :cpp:func:`set_data() <mu::tiny::mock::Support::set_data>`, custom comparator, scoped mock,
  :cpp:func:`crash_on_failure() <mu::tiny::mock::Support::crash_on_failure>`,
  :cpp:func:`disable() <mu::tiny::mock::Support::disable>`/:cpp:func:`enable() <mu::tiny::mock::Support::enable>`,
  :cpp:func:`ignore_other_calls() <mu::tiny::mock::Support::ignore_other_calls>`
- :source:`EventDispatcher.test.cpp <examples/tests/EventDispatcher.test.cpp>`
  — real-world example: virtual mock class, custom comparator in :cpp:func:`setup() <mu::tiny::test::Test::setup>`,
  :cpp:func:`with_parameter_of_type() <mu::tiny::mock::ExpectedCall::with_parameter_of_type>`
