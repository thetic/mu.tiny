cmake_minimum_required(VERSION 3.29)

project(mutinyExample)

if(PROJECT_IS_TOP_LEVEL)
    include(FetchContent)
    FetchContent_Declare(
        mu.tiny
        GIT_REPOSITORY https://github.com/thetic/mu.tiny.git
        GIT_TAG v0.8.1
        FIND_PACKAGE_ARGS 0.8
    )
    FetchContent_MakeAvailable(mu.tiny)
endif()

include(CTest)

if(NOT DEFINED MUTINY_USE_STD_CPP_LIB)
    set(MUTINY_USE_STD_CPP_LIB ON)
endif()

add_library(ExampleLib)

target_sources(ExampleLib
    PRIVATE
        src/CircularBuffer.cpp
        src/Printer.cpp
        src/hello.c
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/CircularBuffer.hpp
            include/Printer.hpp
            include/hello.h
)

if(MUTINY_USE_STD_CPP_LIB)
    target_sources(ExampleLib
        PRIVATE src/EventDispatcher.cpp
        PUBLIC FILE_SET HEADERS BASE_DIRS include
            FILES include/EventDispatcher.hpp
    )
endif()

target_compile_features(ExampleLib PUBLIC cxx_std_11)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
