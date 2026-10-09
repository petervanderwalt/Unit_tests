# Confirmed defects in pinned core

When updating the pinned core, run the regressions before removing exceptions.
For a fixed defect, keep its regression test and remove its CMake `WILL_FAIL` and
`known_bug` properties so it becomes an ordinary passing test. Record the fix date
and core revision here. Each entry below links to its test and exception.
All expected-failure blocks live in [known_bugs.cmake](known_bugs.cmake), so
adding host configurations does not move the linked exceptions.
For VFS, also replace the diagnostic-wrapper registration with a direct executable
test; the fixed test must pass under sanitizers.

## UTF-8 ASCII encoding

**Marked known:** 2026-10-08

**Regression:** [test](cases/utf8/ascii.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L5).

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

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L9).

Decimal zero suffix: `read_uint("42.000")` returns 42000 instead of 42.

`nuts_bolts.read_uint_decimal_zeros` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## read uint maximum

**Marked known:** 2026-10-08

**Regression:** [test](cases/nuts_bolts/read_uint_maximum.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L13).

UINT32_MAX parsing: the final 5 is dropped, producing 429496729.

`nuts_bolts.read_uint_maximum` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## datetime century leap

**Marked known:** 2026-10-08

**Regression:** [test](cases/nuts_bolts/datetime_century_leap.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L17).

Year 2000 is a Gregorian leap year, but the date parser rejects February 29.

`nuts_bolts.datetime_century_leap` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## VFS close use-after-free

**Marked known:** 2026-10-08

**Regression:** [test](cases/fs_ram/close_lifetime.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L21).

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

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L25).

`ngc_named_param_set("_metric", 0)` should reject the assignment with NULL.
At `core/ngc_params.c:855`, it forms `&rw_param->value` while `rw_param` is NULL
for predefined parameters, returning an invalid non-null pointer.
`ngc_params.named_builtin_read_only` retains the correct NULL expectation and is
a labelled expected failure until the pinned core is fixed.

## Embedded filesystem seek

**Marked known:** 2026-10-08

**Regression:** [test](cases/fs_embedded/seek_within_file.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L29).

`vfs_seek(file, 2)` on a five-byte embedded file returns -1 and leaves the
position unchanged. `core/fs_embedded.c:120` reverses the valid-offset test;
line 123 also returns failure for valid offsets. The correct seek expectation
is retained in `fs_embedded.seek_within_file`, labelled as an expected failure.

## Secondary stepper finite-move overshoot

**Marked known:** 2026-10-08

**Regression:** [test](cases/stepper2/finite_polled_move.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L33).

`st2_motor_move(motor, 10, 100, Stepper2_Steps)` emits 12 pulses and leaves
position at 12 instead of 10 in polling mode. The acceleration/deceleration
state transitions in `core/stepper2.c:501-530` continue into the unconditional
step output at line 554. The regression retains the correct 10-pulse expectation.

## JSON escaped-string serialization

**Marked known:** 2026-10-08

**Regression:** [test](cases/stream_json/escaped_quote.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L37).

Serializing the string `a"b` should preserve both letters and escape the quote.
`json_add_string()` instead loses text and produces invalid JSON. The escape path
at `core/stream_json.c:211-219` writes the wrong string spans and escape bytes.
The regression retains the correct JSON expectation. Its bounded output fixture
tracks byte counts explicitly so embedded NUL bytes cannot hide malformed output.

## Device filesystem read byte count

**Marked known:** 2026-10-09

**Regression:** [test](cases/fs_device/read_byte_count.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L41).

Reading three available bytes transfers `abc` correctly but returns `SIZE_MAX`
instead of 3. `core/fs_device.c:151` decrements the unsigned counter through zero
and line 155 returns the underflowed counter. The regression verifies content,
EOF, and the correct byte count; retain the test when removing the exception.

## Quadratic spline rejects a valid J-only control offset

**Marked known:** 2026-10-09

**Regression:** [test](cases/gcode/quadratic_nonzero_j_accepted.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L45).

`G5.1X1Y1I0J1F100` defines a valid quadratic spline with a nonzero J control
offset. The parser returns `Status_GcodeValueOutOfRange` (39) instead of
`Status_OK`. The zero-offset check in `core/gcode.c:3861` compares I twice,
so any I=0 input is rejected even when J is nonzero. Check both I and J for
zero. The regression keeps the accepted-command and final-position expectations.

## Delta travel check accepts an unreachable Cartesian target

**Marked known:** 2026-10-09

**Regression:** [test](cases/delta/unreachable_cartesian_target_rejected.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L49).

With the default delta geometry and all axes homed, the Cartesian target
`X1000 Y1000 Z-300` is physically unreachable, yet `grbl.check_travel_limits()`
returns true. `core/kinematics/delta.c:603` tests successful inverse kinematics
as an error, while a failed inverse can leave zero joint angles that pass the
following range checks. Reject inverse-kinematics failure before checking the
joint-angle bounds. The regression retains the correct rejection expectation.

## RTCP rotary segmentation overshoots the endpoint

**Marked known:** 2026-10-09

**Regression:** [test](cases/rtcp_ac/rotary_segment_endpoint.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L53).

With both rotary centers at zero, an RTCP move from the origin to `X10 C90`
should finish at machine position `X0 Y10 C90`. The segmented move finishes
at `X-20 Y0 C180` instead. `core/kinematics/rtcp_ac.c:348` replaces the starting
RTCP position with the endpoint before interpolation at lines 373-377 adds the
move delta. Preserve the original starting position throughout segmentation.
The regression retains the correct machine endpoint and rotary angle.

## Serial DEL editing character is discarded

**Marked known:** 2026-10-09

**Regression:** [test](cases/protocol/serial_delete_edits_block.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L57).

Receiving `G0X12`, DEL (0x7f), then `3` and a newline should edit the command
into `G0X13`. The serial realtime filter discards DEL, so the main loop instead
executes `G0X123`: 9840 X steps instead of 1040 at 80 steps/mm. The default
filter in `core/protocol.c:1011` excludes 0x7f before the main loop's DEL editing
branch can handle it. Retain the intended edited-command endpoint expectation.
