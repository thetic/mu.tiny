if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
    set(MUTINY_HAVE_FENV OFF)
else()
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
endif()

add_executable(ExampleTests
    main.cpp
    CheatSheet.test.cpp
    CircularBuffer.test.cpp
    FEDemo.test.cpp
    hello.test.cpp
    hello.test.c
    IEEE754ExceptionsPlugin.cpp
    MockCheatSheet.test.cpp
    MockDocumentation.test.c
    MockDocumentation.test.cpp
    OrderedTest.test.cpp
    ParameterizedTest.test.cpp
    Printer.test.cpp
    TeamCityOutputPlugin.cpp
    TeamCityTestOutput.test.cpp
)

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

include(mu.tiny)
mutiny_discover_tests(ExampleTests)
