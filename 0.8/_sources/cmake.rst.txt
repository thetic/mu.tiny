CMake Integration
=================

.. default-domain:: cmake

Build Options
-------------

These options control how *mu::tiny* itself is built.  Pass
:cmake:manual:`-D<OPTION>=<value> <cmake:manual:cmake(1)>` on the command
line or configure them in a
:cmake:manual:`CMake presets <cmake:manual:cmake-presets(7)>` file.

.. variable:: MUTINY_USE_STD_CPP_LIB

   Boolean, default ``ON``.  Build *mu::tiny* with the C++ standard library.
   Set to ``OFF`` for bare-metal or freestanding targets where no ``<std>``
   headers are available.

.. variable:: MUTINY_USE_STD_STRING

   Boolean, default ``ON``.  Use ``std::string`` internally instead of
   *mu::tiny*'s own :cpp:class:`String <mu::tiny::String>` class.
   Requires :variable:`MUTINY_USE_STD_CPP_LIB`.

.. variable:: MUTINY_EXAMPLES

   Boolean.  Build the example programs under ``examples/``.  Defaults to
   ``ON`` when *mu::tiny* is the top-level CMake project, ``OFF`` when consumed
   via :cmake:module:`FetchContent <cmake:module:FetchContent>` or
   :cmake:command:`add_subdirectory() <cmake:command:add_subdirectory>`.

.. variable:: MUTINY_BUILD_TESTING

   Boolean.  Build *mu::tiny*'s own test suite.  Defaults to ``ON`` at the top
   level, ``OFF`` when consumed.  Relevant only for *mu::tiny* developers.

Package Variables
-----------------

These variables are set by :cmake:command:`find_package(mutiny) <cmake:command:find_package>`
and are available to consuming projects immediately after that call.

.. variable:: MUTINY_MODULE_PATH

   Path to the directory containing ``Mutiny.cmake``.
   :cmake:command:`find_package(mutiny) <cmake:command:find_package>` also
   appends this directory to :cmake:variable:`CMAKE_MODULE_PATH
   <cmake:variable:CMAKE_MODULE_PATH>` so that ``include(mutiny)`` works
   without any extra setup.

CTest Integration Module
------------------------

.. cmake-module:: ../cmake/mu.tiny.cmake
