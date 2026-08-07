add_executable(mutiny_test
    src/main.cpp
    src/CommandLineArguments.test.cpp
    src/CommandLineTestRunner.test.cpp
    src/Compatibility.test.cpp
    src/CompositeOutput.test.cpp
    src/mutiny_c.test.c
    src/mutiny_c.test.cpp
    src/mutiny_c_fixture.test.cpp
    src/fail_method_c_body.c
    src/ExpectFailTestShell.test.cpp
    src/SkippedShell.test.cpp
    src/MockComparatorCopier.test.cpp
    src/MockFailureReporter.cpp
    src/MockFixedWidthTypes.test.cpp
    src/MockHierarchy.test.cpp
    src/MockNamedValue.test.cpp
    src/MockNamedValueComparatorsAndCopiersRepository.test.cpp
    src/MockParameter.test.cpp
    src/MockReturnValue.test.cpp
    src/MockStrictOrder.test.cpp
    src/MockSupport.test.cpp
    src/MockSupport.c
    src/MockSupportPlugin.test.cpp
    src/MockSupportWithFixture.test.cpp
    src/MockSupport_c.test.cpp
    src/OrderedTestC.test.c
    src/Ordered.test.cpp
    src/OrderedTestC.test.cpp
    src/Preprocessor.test.cpp
    src/SetPointerPlugin.test.cpp
    src/String.test.cpp
    src/StringView.test.cpp
    src/TestFailure.test.cpp
    src/TestFailureNaN.test.cpp
    src/TestFilter.test.cpp
    src/JUnitOutput.test.cpp
    src/TapOutput.test.cpp
    src/TestInstaller.test.cpp
    src/TestOutput.test.cpp
    src/TestPlugin.test.cpp
    src/TestRegistry.test.cpp
    src/TestResult.test.cpp
    src/Shell.test.cpp
    src/TestShellMacros.test.cpp
    src/TestShellPointerArray.test.cpp
    src/TestShellProperty.test.cpp
    src/TestShellStringMacros.test.cpp
)

target_link_libraries(mutiny_test
    PRIVATE
        mu::tiny
)

include(mu.tiny)
mutiny_discover_tests(mutiny_test)

get_cmake_property(_mutiny_is_multi_config GENERATOR_IS_MULTI_CONFIG)
set(_mutiny_try_compile_arg "")
if(CMAKE_TRY_COMPILE_TARGET_TYPE)
    set(_mutiny_try_compile_arg
        "-DCMAKE_TRY_COMPILE_TARGET_TYPE=${CMAKE_TRY_COMPILE_TARGET_TYPE}"
    )
endif()
add_test(
    NAME mutiny_discover_tests
    COMMAND ${CMAKE_COMMAND}
        -D "RunCMake_GENERATOR=${CMAKE_GENERATOR}"
        -D "RunCMake_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
        -D "RunCMake_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
        -D "RunCMake_GENERATOR_IS_MULTI_CONFIG=${_mutiny_is_multi_config}"
        -D "RunCMake_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        -D "RunCMake_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/cmake"
        -D "MUTINY_MODULE_PATH=${MUTINY_MODULE_PATH}"
        -D "_MUTINY_DISCOVERY_SCRIPT=${_MUTINY_DISCOVERY_SCRIPT}"
        -D "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        ${_mutiny_try_compile_arg}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunCMakeTest.cmake"
)
