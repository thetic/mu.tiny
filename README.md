<img src="docs/_static/logo.png" alt="mu::tiny" width="300">

[![Basic builds](https://github.com/thetic/mu.tiny/actions/workflows/basic.yml/badge.svg)](https://github.com/thetic/mu.tiny/actions/workflows/basic.yml)
[![Coverage Status](https://coveralls.io/repos/github/thetic/mu.tiny/badge.svg)](https://coveralls.io/github/thetic/mu.tiny)
[![Documentation](https://img.shields.io/badge/docs-online-blue)](https://thetic.github.io/mu.tiny/)

---

_mu::tiny_ is a C/C++ unit testing and mocking framework suited for embedded and
low-resource targets.

## Adding mu::tiny to Your Project

The recommended approach uses CMake
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
with `FIND_PACKAGE_ARGS`. This first attempts
[`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html);
if _mu::tiny_ is not installed, CMake fetches and builds it from source
automatically:

```cmake
cmake_minimum_required(VERSION 3.24)
project(my_tests)

include(FetchContent)
FetchContent_Declare(
    mu.tiny
    GIT_REPOSITORY    https://github.com/thetic/mu.tiny.git
    GIT_TAG           v0.9.0
    FIND_PACKAGE_ARGS 0.9
)
FetchContent_MakeAvailable(mu.tiny)

add_executable(my_tests main.cpp widget.test.cpp)
target_link_libraries(my_tests PRIVATE mu::tiny)

include(CTest)
include(mu.tiny)
mutiny_discover_tests(my_tests)
```

If _mu::tiny_ is already installed and you prefer not to use
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html), use
[`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html) directly:

```cmake
find_package(mu.tiny 0.9 REQUIRED)
```

## Headers

All public headers live under `include/mu/tiny/`.
The main headers you'll use:

| Header                               | Purpose                                                            |
| ------------------------------------ | ------------------------------------------------------------------ |
| `mu/tiny/test.hpp`                   | Test and assertion macros (`TEST_GROUP`, `TEST`, `CHECK`, etc.)    |
| `mu/tiny/mock.hpp`                   | Mock framework (`mu::tiny::mock::mock`, `mu::tiny::mock::Support`) |
| `mu/tiny/test.h`                     | C interface                                                        |
| `mu/tiny/test/CommandLineRunner.hpp` | `mu::tiny::test::CommandLineRunner` (`main()` runner)              |
| `mu/tiny/test/Ordered.hpp`           | `TEST_ORDERED` macro                                               |

## Writing `main`

Every test executable needs a `main()`.
The simplest form uses `mu::tiny::test::CommandLineRunner`:

```cpp
#include "mu/tiny/test/CommandLineRunner.hpp"

using mu::tiny::test::CommandLineRunner;

int main(int argc, char** argv)
{
    return CommandLineRunner::run_all_tests(argc, argv);
}
```

To add plugins (e.g. `mu::tiny::test::JUnitOutputPlugin`,
`mu::tiny::test::SetPointerPlugin`,
`mu::tiny::mock::SupportPlugin`), call
`CommandLineRunner::install_plugin()` before `run_all_tests()`:

```cpp
#include "mu/tiny/test/CommandLineRunner.hpp"
#include "mu/tiny/test/JUnitOutputPlugin.hpp"
#include "mu/tiny/mock/SupportPlugin.hpp"

using mu::tiny::test::CommandLineRunner;

int main(int argc, char** argv)
{
    mu::tiny::test::JUnitOutputPlugin junit;
    mu::tiny::mock::SupportPlugin mock_plugin;
    CommandLineRunner::install_plugin(junit);
    CommandLineRunner::install_plugin(mock_plugin);
    return CommandLineRunner::run_all_tests(argc, argv);
}
```

## Your First Test

See [examples/tests/CheatSheet.test.cpp](examples/tests/CheatSheet.test.cpp):

- `TEST_GROUP` declares a group;
  the `struct` implicitly inherits from `mu::tiny::test::Test`.
  `setup()` runs before each test body;
  `teardown()` runs after.
- `TEST(group, name)` defines a single test.
- A failing assertion immediately exits the test body.

## Running Tests

Run the binary directly:

```sh
./build/tests/my_tests                  # run all
./build/tests/my_tests -v               # verbose: print each test name
./build/tests/my_tests -g CheatSheet    # only group "CheatSheet"
./build/tests/my_tests -n TestName      # only tests whose name contains this
```

Via CTest (after `mutiny_discover_tests` in CMakeLists):

```sh
ctest --preset GNU
ctest --preset GNU -R CheatSheet
```

See [docs/command-line-reference.rst](docs/command-line-reference.rst) for all flags.

## Examples

| File                                                              | Demonstrates                                                                         |
| ----------------------------------------------------------------- | ------------------------------------------------------------------------------------ |
| [CheatSheet.test.cpp](examples/tests/CheatSheet.test.cpp)         | Minimal `TEST_GROUP` + `TEST` with `setup()`, `MUTINY_PTR_SET`, and assertion macros |
| [CircularBuffer.test.cpp](examples/tests/CircularBuffer.test.cpp) | Full group with setup/teardown, helper methods, and multiple assertion styles        |

## Building mu::tiny

_mu::tiny_ uses CMake. Clone the repository and configure:

```sh
cmake -B build
cmake --build build
cmake --install build  # optional system install
```

## Attribution

_mu::tiny_ is descended from [CppUTest](https://github.com/cpputest/cpputest).
Users looking for more robust support of older language standards, or legacy compilers and platforms should consider using _CppUTest_ instead.
