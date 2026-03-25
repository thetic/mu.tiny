include(CheckSourceRuns)
# Verify that FP exception flags are actually raised at runtime, not just that
# fenv.h compiles.  Soft-float targets (e.g. ARM without -mfpu/-mfloat-abi)
# compile fenv.h cleanly but never write FPSCR, so fetestexcept() returns 0.
check_source_runs(CXX
    "#include <fenv.h>
     int main(void) {
         volatile float z = 0.0f;
         volatile float r = 1.0f / z;
         (void)r;
         return fetestexcept(FE_DIVBYZERO) ? 0 : 1;
     }"
    MUTINY_HAVE_FENV
)

add_executable(ExampleTests
    main.cpp
    hello.test.cpp
    hello.test.c
    OrderedTest.test.cpp
)

# The ATmega2560 has only 8 KB SRAM.  On AVR, every string literal (including
# test-group names, test names, and __FILE__ paths) lands in SRAM, so the
# usable test set is constrained by that budget.  Files below are included on
# all non-AVR targets.  On AVR the hello + OrderedTest set uses ~7.2 KB SRAM,
# leaving ~1 KB of headroom for the framework's stack.
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "avr")
    target_sources(ExampleTests PRIVATE
        CheatSheet.test.cpp
        CircularBuffer.test.cpp
        FEDemo.test.cpp
        IEEE754ExceptionsPlugin.cpp
        MockCheatSheet.test.cpp
        MockDocumentation.test.c
        MockDocumentation.test.cpp
        Printer.test.cpp
        TeamCityOutputPlugin.cpp
        TeamCityTestOutput.test.cpp
    )
endif()

if(MUTINY_USE_STD_CPP_LIB)
    target_sources(ExampleTests PRIVATE EventDispatcher.test.cpp)
endif()

target_include_directories(ExampleTests
    PRIVATE
        .
)

target_compile_definitions(ExampleTests
    PRIVATE MUTINY_HAVE_FENV=$<BOOL:${MUTINY_HAVE_FENV}>
)

target_link_libraries(ExampleTests
    PRIVATE
        ExampleLib
        mu::tiny
)

include(mutiny)
mutiny_discover_tests(ExampleTests)
