# Confirmed defects in pinned core

When updating the pinned core, run the regressions before removing exceptions.
For a fixed defect, keep its regression test and remove its CMake `WILL_FAIL` and
`known_bug` properties so it becomes an ordinary passing test. Record the fix date
and core revision here. Each entry below links to its test and exception.
All expected-failure blocks live in [known_bugs.cmake](known_bugs.cmake), so
adding host configurations does not move the linked exceptions.
For memory defects, also replace the diagnostic-wrapper registration with a direct
executable test; the fixed test must pass under sanitizers. For feature compile
defects, remove the diagnostic matcher and retain an ordinary successful compile
check. Recovering sanitizer diagnostics have a separate, narrowly scoped
[manifest](known_sanitizer_diagnostics.json); remove its linked entries when fixed.

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
filter in `core/protocol.c:1014` excludes 0x7f before the main loop's DEL editing
branch can handle it. Retain the intended edited-command endpoint expectation.

## M66 digital input port wraps before validation

**Marked known:** 2026-10-09

**Regression:** [test](cases/gcode/m66_large_port_rejected.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L61).

With two digital inputs registered, `M66P256L0` should reject unavailable port
256 with `Status_GcodeValueOutOfRange` (39). It instead succeeds and reads
physical input 0. `core/gcode.c:2576` casts P to an eight-bit port number before
checking the available input count; 256 wraps to zero. Validate the full numeric
value before narrowing it. The regression keeps rejection and zero-read expectations.

## Analog output commands truncate fractional values

**Marked known:** 2026-10-09

**Regression:** [test](cases/gcode/m68_preserves_fractional_output.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L64).

`M68E1Q1.25` passes 1.0 to the analog output callback instead of 1.25.
The callback accepts a float (`core/ioports.h:49`), but `output_command_t.value`
is an `int32_t` (`core/gcode.h:312`). Assigning Q at `core/gcode.c:2604`
truncates fractional values before immediate output at line 4201 or synchronized
M67 output in `core/stepper.c:515`. Preserve the numeric value through command
storage. The regression retains the fractional output expectation.

## Named parameter parser accepts one character beyond its maximum

**Marked known:** 2026-10-09

**Regression:** [test](cases/ngc_expr/name_over_maximum_rejected.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L68).

`ngc_read_name()` accepts a 31-character name although `NGC_MAX_PARAM_LENGTH`
is 30. The `len <= NGC_MAX_PARAM_LENGTH` check at `core/ngc_expr.c:602`
allows the extra character. The terminating null at line 609 then requires
32 bytes, exceeding the 31-byte local buffer used by `ngc_read_parameter()`
at line 648. Reject the overlong name before writing beyond the supported
length. The regression deliberately allocates extra storage so it can assert
correct rejection without relying on a memory-corruption crash.

## NGC position parameters use display units instead of G20/G21

**Marked known:** 2026-10-09

**Regression:** [G20 test](cases/ngc_params/absolute_position_parameter_inches.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L72).

At 80 steps/mm, an X position of 2032 steps is 25.4 mm, or one inch.
After executing `G20`, `#<_abs_x>` should return 1.0; the pinned core
returns 25.4 with the default display setting. NGC parameter units must
follow `gc_state.modal.units_imperial` (`G20`/`G21`), independently of `$13`.
Both `_convert_pos()` variants in `core/ngc_params.c:144` and
`core/ngc_params.c:151` instead use `settings.flags.report_inches`, and
multiply by 25.4 when that display flag is enabled. Use modal units and
divide by 25.4 for imperial linear positions, retaining rotary-axis
exemptions in multi-axis builds. The G20 regression leaves `$13` untouched;
the separate G21 metric case expects 25.4 mm.
The [display-independence regression](cases/ngc_params/absolute_position_metric_ignores_display_inches.c)
sets G21 and `$13=1`; it expects 25.4 but gets 645.16. Remove its
[separate exception](known_bugs.cmake#L84) along with the G20 exception
when this defect is fixed.

## Spindle enumeration omits a spindle enabled through the runtime API

**Marked known:** 2026-10-09

**Regression:** [test](cases/multi_spindle/report_machine_format_shows_enabled_slots.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L76).

With three registered spindles and two active slots, `spindle_enable(1)`
returns slot 1. `spindle_is_enabled(1)` is true and `spindle_get(1)->id`
is 1, but the machine-readable report emits `[SPINDLE:1|-|...]`, marking
that physical spindle disabled. `spindle_get_num()` in
`core/spindle_control.c:338-355` derives the logical slot from setting values
rather than the current enabled spindle array. Enumeration at lines 419-420
then derives both `num` and `enabled` from that mapping. Report the actual
runtime slot assignments, or explicitly synchronize setting-based mappings
when the public enable API succeeds. The regression retains slot 1 in the
expected report and separately verifies that activation really succeeded.

## Negative broadcast spindle selector fails on Linux

**Marked known:** 2026-10-09

**Regression:** [test](cases/multi_spindle/gcode_m5_broadcast_stops_enabled_spindles.c).

**Remove exception when fixed:** [Linux-only CMake registration](known_bugs.cmake#L80).

`M5$-1` is the supported command to stop all enabled spindle slots, but both
Linux GCC and Clang reject it. `core/gcode.c:1486` converts the negative
floating word value to an unsigned integer; line 1487 then converts the
resulting negative fractional calculation to an unsigned mantissa. Linux
Clang additionally reports undefined floating-to-unsigned conversions at
both lines. Compute a valid magnitude or signed fractional part before
checking whether the selector is integral. These generic conversions also
run for other negative word values.

The same correct regression passes on Windows, so only Linux currently has
an expected-failure exception. The test still requires successful broadcast
stop and zero speed on both spindle outputs. Windows success does not prove
the underlying unsigned conversions are defined.

Recovering sanitizer diagnostics from these two conversions are explicitly listed
in [the diagnostic manifest](known_sanitizer_diagnostics.json). Remove that entry
when the conversions are fixed. The report separates affected assertion passes
from clean passes and fails on any unrecognized sanitizer diagnostic.

## Too-short step pulse returns generic setting error

**Marked known:** 2026-10-09

**Regression:** [test](cases/settings/pulse_width_minimum_reports_specific_error.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L88).

With a driver minimum of 2 microseconds, storing `$0=1.9` correctly rejects
and preserves the previous pulse width, but returns
`Status_SettingValueOutOfRange` (52) instead of `Status_SettingStepPulseMin`
(6). In `core/settings.c:3574`, `setting` is a pointer to the setting
metadata, while `Setting_PulseMicroseconds` is setting ID 0. Comparing the
pointer with that ID makes the intended specific-error mapping unreachable.
Compare `setting->id` with the ID instead. The regression verifies unchanged
timing and no change notification before checking the correct status code.

## Settings recovery overreads the default build-info string

**Marked known:** 2026-10-09

**Regression:** [test](cases/settings/initialize_bad_version_restores_defaults.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L92),
[diagnostic matcher](../scripts/settings_recovery_regression.py), and its
special test command in [CMakeLists.txt](../CMakeLists.txt).

Invalid stored settings trigger `settings_init()` → `settings_restore()`.
At `core/settings.c:3041`, restoration calls `settings_write_build_info(BUILD_INFO)`.
The default `BUILD_INFO` in `core/config.h:79` is an empty string occupying
one byte, but `settings_write_build_info()` at `core/settings.c:2825` asks
the NVS driver to read `sizeof(stored_line_t)` (70 bytes) from it. ASan confirms
a global buffer overread. Copy the build-info string into a bounded, padded
record before writing it. The test retains successful recovery, persisted
version, defaults, and notification expectations. Its wrapper accepts only
the matching ASan overread and both source frames; unrelated crashes fail.
Without sanitizers, this memory regression is explicitly skipped.

## Auxiliary pullup setting feature does not compile

**Marked known:** 2026-10-09

**Regression:** [compile case](compile/ioports_aux_pullup.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L96)
and [diagnostic matcher](../scripts/aux_pullup_compile_regression.py);
retain an ordinary successful syntax check for this feature.

Compiling `core/ioports.c` with `AUX_SETTINGS_PULLUP=1` fails at line 1446:
the pullup-setting metadata references `digital.in.port_names`, but `digital`
is not declared in the current implementation. Use the current input port
metadata, as the adjacent inversion setting does. Clang also reports the
resulting incomplete settings array at line 1646. The compile regression
expects this feature to compile and recognizes only these specific errors;
fixes and unrelated build errors force review of the exception.

## Rejected negative spindle speed changes RPM/CSS mode

**Marked known:** 2026-10-09

**Regression:** [test](cases/gcode/css_negative_speed_preserves_rpm_mode.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L100).

With a variable-speed spindle in lathe mode, `G96S-1` correctly returns
`Status_NegativeValue` but changes the existing RPM mode to CSS mode.
`core/gcode.c:2492` assigns `sspindle->rpm_mode` during validation, before
the negative S value is rejected at line 2498. Defer that modal-state update
until the block has passed validation. The regression checks the expected
negative-value error and retains the original RPM-mode expectation; it
prints the observed mode 1 versus expected mode 0.

The reverse transition has the same defect: after a valid `G96S100`,
`G97S-1` returns `Status_NegativeValue` but changes CSS mode to RPM mode.
Keep the [G97 regression](cases/gcode/g97_negative_speed_preserves_css_mode.c)
and remove its [separate exception](known_bugs.cmake#L104) when fixed.

## Axis-word validation accepts a valueless axis

**Marked known:** 2026-10-09

**Regression:** [test](cases/gcode/claim_axis_words_validation_retains_valueless_axis.c).

**Remove exception when fixed:** [CMake registration](known_bugs.cmake#L108).

`gc_claim_axis_words()` with validation enabled for X accepts an X word whose
value is `NaN`, clears that word from the block, and reports X as claimed.
The regression expects X to remain unclaimed while the numeric Y word is
claimed. `core/gcode.c:392` uses the mutating `bit_true` macro inside the
validation condition. The macro expands to an unparenthesized `|=` assignment;
its existing nonzero mask makes the expression true even when `!isnan()` is
false. Use an explicit validation condition with the intended mask semantics,
and retain the numeric-value check before claiming a validated axis.
