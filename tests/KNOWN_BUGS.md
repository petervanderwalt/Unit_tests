# Confirmed defects in pinned core

## UTF-8 ASCII encoding

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

Decimal zero suffix: `read_uint("42.000")` returns 42000 instead of 42.

`nuts_bolts.read_uint_decimal_zeros` is a labelled expected failure, with the same
removal policy as the ASCII test above.

## read uint maximum

UINT32_MAX parsing: the final 5 is dropped, producing 429496729.

`nuts_bolts.read_uint_maximum` is a labelled expected failure, with the same
removal policy as the ASCII test above.
