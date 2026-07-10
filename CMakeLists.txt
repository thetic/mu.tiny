cmake_minimum_required(VERSION 3.29)

file(READ "cfg/VERSION" MUTINY_VERSION_RAW)
string(STRIP "${MUTINY_VERSION_RAW}" MUTINY_VERSION)

project(mutiny
    VERSION ${MUTINY_VERSION}
    LANGUAGES CXX
)

include(CTest)

include(CMakeDependentOption)
option(MUTINY_USE_STD_CPP_LIB "Use the standard C++ library" ON)
cmake_dependent_option(MUTINY_USE_STD_STRING "Use std::string"
    ON "MUTINY_USE_STD_CPP_LIB" OFF)
cmake_dependent_option(MUTINY_BUILD_TESTING "Build tests for mutiny"
    ${PROJECT_IS_TOP_LEVEL}
    "BUILD_TESTING;EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests"
    OFF)
cmake_dependent_option(MUTINY_EXAMPLES "Compile and make examples?"
    ON
    "PROJECT_IS_TOP_LEVEL;EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples"
    OFF)


set(MUTINY_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    CACHE PATH "Path to the mutiny CMake modules directory" FORCE
)
list(APPEND CMAKE_MODULE_PATH "${MUTINY_MODULE_PATH}")
if(NOT PROJECT_IS_TOP_LEVEL)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
endif()

add_library(mutiny)

configure_file(
    include/mu/tiny/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/mu/tiny/version.h
)

include(GenerateExportHeader)
generate_export_header(mutiny
    EXPORT_FILE_NAME include/mu/tiny/export.h
    INCLUDE_GUARD_NAME INCLUDED_MU_TINY_EXPORT_H
)

target_sources(mutiny
    PRIVATE
        src/mock/ActualCallTrace.cpp
        src/mock/CheckedActualCall.cpp
        src/mock/CheckedExpectedCall.cpp
        src/mock/ExpectedCallsList.cpp
        src/mock/Failure.cpp
        src/mock/IgnoredExpectedCall.cpp
        src/mock/NamedValue.cpp
        src/mock/NamedValueComparatorsAndCopiersRepository.cpp
        src/mock/NamedValueList.cpp
        src/mock/SupportPlugin.cpp
        src/mock/mock.c.cpp
        src/mock/mock.cpp
        src/String.cpp
        src/StringView.cpp
        src/test/CommandLineArguments.cpp
        src/test/CommandLineRunner.cpp
        src/test/CompositeOutput.cpp
        src/test/ConsoleOutput.cpp
        src/test/ExecFunctionShell.cpp
        src/test/ExpectFailShell.cpp
        src/test/Failure.cpp
        src/test/Filter.cpp
        src/test/GroupLocation.cpp
        src/test/SkippedShell.cpp
        src/test/Installer.cpp
        src/test/JUnitOutput.cpp
        src/test/JUnitOutputPlugin.cpp
        src/test/NullPlugin.cpp
        src/test/TapOutput.cpp
        src/test/TapOutputPlugin.cpp
        src/test/Ordered.cpp
        src/test/Output.cpp
        src/test/Plugin.cpp
        src/test/Registry.cpp
        src/test/Result.cpp
        src/test/SetPointerPlugin.cpp
        src/test/Shell.cpp
        src/test/ShellPointerArray.cpp
        src/StringCollection.cpp
        src/test/Terminator.cpp
        src/test/Test.cpp
        src/test/TestingFixture.cpp
        src/test/jump_buffer.cpp
        src/math.cpp
        src/test/test.c.cpp
        src/time.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS
            include
            "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES
            include/mu/tiny/mock.h
            include/mu/tiny/mock.hpp
            include/mu/tiny/mock/ActualCall.hpp
            include/mu/tiny/mock/ExpectedCall.hpp
            include/mu/tiny/mock/Failure.hpp
            include/mu/tiny/mock/NamedValue.hpp
            include/mu/tiny/mock/NamedValueComparator.hpp
            include/mu/tiny/mock/NamedValueComparatorsAndCopiersRepository.hpp
            include/mu/tiny/mock/NamedValueCopier.hpp
            include/mu/tiny/mock/SupportPlugin.hpp
            include/mu/tiny/String.hpp
            include/mu/tiny/StringView.hpp
            include/mu/tiny/test.h
            include/mu/tiny/test.hpp
            include/mu/tiny/test/CommandLineArguments.hpp
            include/mu/tiny/test/CommandLineRunner.hpp
            include/mu/tiny/test/ConsoleOutput.hpp
            include/mu/tiny/test/ExecFunction.hpp
            include/mu/tiny/test/ExpectFailShell.hpp
            include/mu/tiny/test/Failure.hpp
            include/mu/tiny/test/Filter.hpp
            include/mu/tiny/test/GroupLocation.hpp
            include/mu/tiny/test/SkippedShell.hpp
            include/mu/tiny/test/Installer.hpp
            include/mu/tiny/test/JUnitOutputPlugin.hpp
            include/mu/tiny/test/Ordered.hpp
            include/mu/tiny/test/TapOutputPlugin.hpp
            include/mu/tiny/test/Output.hpp
            include/mu/tiny/test/Plugin.hpp
            include/mu/tiny/test/Registry.hpp
            include/mu/tiny/test/Result.hpp
            include/mu/tiny/test/SetPointerPlugin.hpp
            include/mu/tiny/test/Shell.hpp
            include/mu/tiny/test/ShellPointerArray.hpp
            include/mu/tiny/test/StringBufferOutput.hpp
            include/mu/tiny/test/Terminator.hpp
            include/mu/tiny/test/Test.hpp
            include/mu/tiny/test/TestingFixture.hpp
            include/mu/tiny/features.hpp
            include/mu/tiny/time.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/mu/tiny/version.h
            ${CMAKE_CURRENT_BINARY_DIR}/include/mu/tiny/export.h
    PRIVATE FILE_SET private_headers TYPE HEADERS
        BASE_DIRS src
        FILES
            src/mu/tiny/mock/ActualCallTrace.hpp
            src/mu/tiny/mock/CheckedActualCall.hpp
            src/mu/tiny/mock/CheckedExpectedCall.hpp
            src/mu/tiny/mock/ExpectedCallsList.hpp
            src/mu/tiny/mock/IgnoredActualCall.hpp
            src/mu/tiny/mock/IgnoredExpectedCall.hpp
            src/mu/tiny/mock/NamedValueList.hpp
            src/mu/tiny/test/CompositeOutput.hpp
            src/mu/tiny/test/ExecFunctionShell.hpp
            src/mu/tiny/test/JUnitOutput.hpp
            src/mu/tiny/test/NullPlugin.hpp
            src/mu/tiny/test/TapOutput.hpp
            src/mu/tiny/StringCollection.hpp
            src/mu/tiny/test/jump_buffer.hpp
            src/mu/tiny/math.hpp
)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(gettimeofday "sys/time.h" MUTINY_HAVE_GETTIMEOFDAY)

target_compile_features(mutiny
    PUBLIC cxx_std_11
)

target_compile_definitions(mutiny
    PUBLIC
        MUTINY_USE_STD_CPP_LIB=$<BOOL:${MUTINY_USE_STD_CPP_LIB}>
        MUTINY_USE_STD_STRING=$<BOOL:${MUTINY_USE_STD_STRING}>
    PRIVATE
        $<$<BOOL:${MUTINY_HAVE_GETTIMEOFDAY}>:MUTINY_HAVE_GETTIMEOFDAY>
)

set_target_properties(mutiny PROPERTIES
    EXPORT_NAME tiny
    CXX_VISIBILITY_PRESET hidden
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
)

add_library(mu::tiny ALIAS mutiny)

if (MUTINY_BUILD_TESTING)
    add_subdirectory(tests)
endif ()

if (MUTINY_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(GNUInstallDirs)

    install(
        TARGETS mutiny
        EXPORT mu.tiny-Targets
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT mutiny_Runtime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT mutiny_Runtime
            NAMELINK_COMPONENT mutiny_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT mutiny_Development
        FILE_SET HEADERS
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT mutiny_Development
    )

    configure_file(cfg/mu.tiny.pc.in ${CMAKE_CURRENT_BINARY_DIR}/mu.tiny.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/mu.tiny.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT mutiny_Development
    )

    include(CMakePackageConfigHelpers)

    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
    set(MODULES_DIR "${MUTINY_MODULE_PATH}")

    set(MUTINY_CONFIG_DEST "${LIB_INSTALL_DIR}/mu.tiny/cmake")

    # CMake package config files (install tree).
    configure_package_config_file(cfg/mu.tiny-config.cmake.install.in
        ${CMAKE_CURRENT_BINARY_DIR}/install/mu.tiny-config.cmake
        INSTALL_DESTINATION "${MUTINY_CONFIG_DEST}"
        PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/install/mu.tiny-configVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/install/mu.tiny-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/install/mu.tiny-configVersion.cmake
        DESTINATION "${MUTINY_CONFIG_DEST}"
    )
    install(
        EXPORT mu.tiny-Targets
        NAMESPACE mu::
        DESTINATION "${MUTINY_CONFIG_DEST}"
    )
    install(
        FILES
            ${MUTINY_MODULE_PATH}/mu.tiny.cmake
            ${MUTINY_MODULE_PATH}/_mutiny_discovery.cmake
        DESTINATION "${MUTINY_CONFIG_DEST}/Modules"
    )

    # CMake package config files (build tree).
    configure_package_config_file(cfg/mu.tiny-config.cmake.build.in
        ${PROJECT_BINARY_DIR}/mu.tiny-config.cmake
        INSTALL_DESTINATION ${PROJECT_BINARY_DIR}
        PATH_VARS INCLUDE_DIR PROJECT_BINARY_DIR MODULES_DIR
    )
    write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/mu.tiny-configVersion.cmake
        COMPATIBILITY SameMajorVersion
    )

    execute_process(
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    file(WRITE "${CMAKE_BINARY_DIR}/source-extras/GIT_REVISION" "${GIT_REVISION}\n")

    set(CPACK_VERBATIM_VARIABLES YES)
    set(CPACK_SOURCE_INSTALLED_DIRECTORIES
        "${CMAKE_SOURCE_DIR}" "/"
        "${CMAKE_BINARY_DIR}/source-extras" "/"
    )
    set(CPACK_SOURCE_IGNORE_FILES
        "/\\.git/"
        "/\\.github/"
        "/\\.cache/"
        "/\\.claude/"
        "/\\.direnv/"
        "/build/"
        "/docs/"
        "/examples/"
        "/tests/"
        "/cfg/CTestCustom\\.cmake\\.in$"
        "/cfg/lcov\\.cmake$"
        "/cfg/warnings\\.cmake$"
        "/\\.clang-format$"
        "/\\.clang-tidy$"
        "/CMakePresets\\.json$"
        "/CONTRIBUTING\\.md$"
        "/compile_commands\\.json$"
        "/\\.envrc$"
        "/\\.gitignore$"
        "/\\.git-blame-ignore-revs$"
    )
    include(CPack)
endif()
