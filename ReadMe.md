# Unit tests for the grblHAL core

[![Core tests](https://github.com/petervanderwalt/Unit_tests/actions/workflows/tests.yml/badge.svg?branch=master)](https://github.com/petervanderwalt/Unit_tests/actions/workflows/tests.yml)

Host tests compile actual source from the pinned `core` submodule. No CNC
controller is needed. Each new behavior lives in `tests/cases/<module>/<case>.c`
and appears individually in CTest and GitHub Actions. Shared mocks live in
`tests/support/`; unexpected mock calls fail explicitly.

Confirmed upstream defects are documented in [tests/KNOWN_BUGS.md](tests/KNOWN_BUGS.md).
Their labelled regressions reproduce the defect; they are not counted as ordinary
passes in the Actions summary. A fixed core forces review of each exception.

## Run locally

Install CMake, Python 3 and GCC or GNU-compatible Clang for the full suite.
Visual Studio's MSVC runs only the standalone modules: other core headers use
GNU extensions and are explicitly skipped. A portable llvm-mingw compiler works
on Windows without installing or changing the system toolchain.

For a Windows GNU-compatible Clang/Ninja build:

```powershell
cmake -S . -B build-clang -G Ninja -DCMAKE_C_COMPILER=C:/path/to/llvm-mingw/bin/clang.exe -DCMAKE_BUILD_TYPE=Debug
cmake --build build-clang --parallel
ctest --test-dir build-clang --output-on-failure
```

For sanitizers, add `-DENABLE_SANITIZERS=ON` to configure. On Windows, put the
compiler's `bin` directory on `PATH` so its sanitizer DLL can load.

```sh
git clone --recurse-submodules https://github.com/petervanderwalt/Unit_tests.git
cd Unit_tests
# For an existing checkout:
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

Checks remain active in Release builds; they do not depend on C `assert()`.
The existing `src/driver.c` firmware entry point is not part of the host tests.

## GitHub Actions

`.github/workflows/tests.yml` runs on pushes, pull requests and manual dispatch.
GCC and Clang run the tests with address/undefined-behavior sanitizers. A separate
GCC job publishes HTML and XML coverage as an Actions artifact. Coverage describes
the instrumented modules in this configuration. The summary lists every core source file, including gaps. No token or external service is needed.

## Add tests incrementally

1. Add isolated modules and boundary/error cases first (string utilities, parsing
   helpers, expressions). Compile their original `.c` files; do not copy functions.
2. Introduce a small mock HAL for settings/NVS, streams, spindle and coolant.
   Record calls and inject failures, without claiming hardware timing coverage.
3. Test G-code parsing with accepted/rejected commands, modal state and units.
4. Test planner geometry, limits, queued blocks and feed calculations.
5. Add protocol/state-machine sequences: reset, hold/resume, alarms and probing.
6. Add representative axis/feature configurations and hardware integration tests.

Create `tests/cases/<module>/<case>.c`; CMake discovers each case automatically.
Add module dependencies in `add_core_test()` when needed.
Each regression should name the triggering input and check the expected behavior.
Avoid tests that merely reproduce the implementation. Keep commits to the core
submodule deliberate: `git -C core fetch`, check out the desired revision, rerun
tests, then commit the updated submodule pointer here.

PID tests currently specify existing sample-rate behavior. Deadband and
`p_max_error` are not asserted as supported features; the implementation does not
apply them. Zero sample rate needs an upstream-defined contract before testing
an expected result.
