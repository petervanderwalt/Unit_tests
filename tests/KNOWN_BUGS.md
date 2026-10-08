# Confirmed defects in pinned core

When updating the pinned core, run the regressions before removing exceptions.
For a fixed defect, keep its regression test and remove its CMake `WILL_FAIL` and
`known_bug` properties so it becomes an ordinary passing test. Record the fix date
and core revision here. Each entry below links to its test and exception.
For VFS, also replace the diagnostic-wrapper registration with a direct executable
test; the fixed test must pass under sanitizers.

## UTF-8 ASCII encoding

**Marked known:** 2026-10-08

**Regression:** [test](cases/utf8/ascii.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L132).

`utf8.ascii` checks that U+0041 encodes to byte 0x41. At core revision
451a539c6ed0f00499b8b48258f9aa7971fac384, it produces 0xC1.
The ASCII branch uses a six-bit mask; the resulting leading-byte expression adds
0x80 modulo 256. This is a core defect, not a reason to change the expected byte.

CTest marks this one regression `WILL_FAIL` with label `known_bug`. Thus a green
run confirms the defect still reproduces, not that ASCII encoding is correct.
Unexpected success fails CI so the exception must be removed when core is fixed.
Run `ctest --test-dir build -C Debug -L known_bug --output-on-failure` to isolate it.
Run the executable directly to see the original failing assertion.

No core source changes have been made.

## read uint decimal zeros

**Marked known:** 2026-10-08

**Regression:** [test](cases/nuts_bolts/read_uint_decimal_zeros.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L135).

Decimal zero suffix: `read_uint("42.000")` returns 42000 instead of 42.

`nuts_bolts.read_uint_decimal_zeros` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## read uint maximum

**Marked known:** 2026-10-08

**Regression:** [test](cases/nuts_bolts/read_uint_maximum.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L139).

UINT32_MAX parsing: the final 5 is dropped, producing 429496729.

`nuts_bolts.read_uint_maximum` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## datetime century leap

**Marked known:** 2026-10-08

**Regression:** [test](cases/nuts_bolts/datetime_century_leap.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L143).

Year 2000 is a Gregorian leap year, but the date parser rejects February 29.

`nuts_bolts.datetime_century_leap` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## VFS close use-after-free

**Marked known:** 2026-10-08

**Regression:** [test](cases/fs_ram/close_lifetime.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L109).

`vfs_close()` calls the backend close at `core/vfs.c:365`, which frees the file
handle (`core/fs_ram.c:174`). It then reads `file->status.update` at `vfs.c:367`.
Confirmed by native Clang AddressSanitizer with `fs_ram.close_lifetime`.

This regression uses a diagnostic matcher: only a heap-use-after-free summary at
`core/vfs.c:367` or the following notification access at line 368 counts as the
expected defect. The regression mounts RAM visibly and installs a notification
callback so both invalid accesses are exercised. An unrelated crash or unexpected
success fails CI. Without sanitizers this case is skipped, not counted as passed.

## Read-only named parameter assignment

**Marked known:** 2026-10-08

**Regression:** [test](cases/ngc_params/named_builtin_read_only.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L147).

`ngc_named_param_set("_metric", 0)` should reject the assignment with NULL.
At `core/ngc_params.c:855`, it forms `&rw_param->value` while `rw_param` is NULL
for predefined parameters, returning an invalid non-null pointer.
`ngc_params.named_builtin_read_only` retains the correct NULL expectation and is
a labelled expected failure until the pinned core is fixed.

## Embedded filesystem seek

**Marked known:** 2026-10-08

**Regression:** [test](cases/fs_embedded/seek_within_file.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L151).

`vfs_seek(file, 2)` on a five-byte embedded file returns -1 and leaves the
position unchanged. `core/fs_embedded.c:120` reverses the valid-offset test;
line 123 also returns failure for valid offsets. The correct seek expectation
is retained in `fs_embedded.seek_within_file`, labelled as an expected failure.

## Secondary stepper finite-move overshoot

**Marked known:** 2026-10-08

**Regression:** [test](cases/stepper2/finite_polled_move.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L155).

`st2_motor_move(motor, 10, 100, Stepper2_Steps)` emits 12 pulses and leaves
position at 12 instead of 10 in polling mode. The acceleration/deceleration
state transitions in `core/stepper2.c:501–530` continue into the unconditional
step output at line 554. The regression retains the correct 10-pulse expectation.

## JSON escaped-string serialization

**Marked known:** 2026-10-08

**Regression:** [test](cases/stream_json/escaped_quote.c).

**Remove exception when fixed:** [CMake registration](../CMakeLists.txt#L159).

Serializing the string `a"b` should preserve both letters and escape the quote.
`json_add_string()` instead loses text and produces invalid JSON. The escape path
at `core/stream_json.c:211–219` writes the wrong string spans and escape bytes.
The regression retains the correct JSON expectation. Its bounded output fixture
tracks byte counts explicitly so embedded NUL bytes cannot hide malformed output.
