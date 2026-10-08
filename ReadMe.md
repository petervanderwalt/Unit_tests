# Unit tests for the grblHAL core

[![Core tests](https://github.com/petervanderwalt/Unit_tests/actions/workflows/tests.yml/badge.svg?branch=master)](https://github.com/petervanderwalt/Unit_tests/actions/workflows/tests.yml)

Host tests compile the actual source from the pinned `core` submodule. No CNC
controller is needed. The initial suites cover CRC check vectors, empty inputs,
checksum overflow, PID gains, accumulation, changing sample rates, clamps,
configuration changes and reset behavior.

## Run locally

Install CMake and a C compiler (GCC, Clang, or Visual Studio Build Tools).

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
only `crc.c` and `pid.c`, not the entire core. No token or external service is needed.

## Add tests incrementally

1. Add isolated modules and boundary/error cases first (string utilities, parsing
   helpers, expressions). Compile their original `.c` files; do not copy functions.
2. Introduce a small mock HAL for settings/NVS, streams, spindle and coolant.
   Record calls and inject failures, without claiming hardware timing coverage.
3. Test G-code parsing with accepted/rejected commands, modal state and units.
4. Test planner geometry, limits, queued blocks and feed calculations.
5. Add protocol/state-machine sequences: reset, hold/resume, alarms and probing.
6. Add representative axis/feature configurations and hardware integration tests.

Create `tests/test_<module>.c` and add the module to CMake with its dependencies.
Each regression should name the triggering input and check the expected behavior.
Avoid tests that merely reproduce the implementation. Keep commits to the core
submodule deliberate: `git -C core fetch`, check out the desired revision, rerun
tests, then commit the updated submodule pointer here.

PID tests currently specify existing sample-rate behavior. Deadband and
`p_max_error` are not asserted as supported features; the implementation does not
apply them. Zero sample rate needs an upstream-defined contract before testing
an expected result.
