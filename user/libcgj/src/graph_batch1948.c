/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1948: test one bit in a trace flag word.
 *
 * Surface (unique symbols):
 *   int gj_trace_flag_test(uint32_t flags, unsigned bit);
 *     — Return 1 if bit of flags is set (bit 0 = LSB), else 0.
 *       Out-of-range index bit >= 32 yields 0 (no shift UB).
 *   int __gj_trace_flag_test  (alias)
 *   __libcgj_batch1948_marker = "libcgj-batch1948"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1948_marker[] = "libcgj-batch1948";

/* Trace flag word width (bits). */
#define B1948_FLAG_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bit uBit of uFlags is set and in range. */
static int
b1948_flag_test(uint32_t uFlags, unsigned uBit)
{
	if (uBit >= B1948_FLAG_BITS) {
		return 0;
	}
	return ((uFlags >> uBit) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trace_flag_test — test whether one trace flag bit is set.
 *
 * flags: packed trace enable mask
 * bit: bit index (0 = least significant bit)
 *
 * Returns 1 if (flags & (1u << bit)) is non-zero and bit < 32;
 * returns 0 if bit >= 32 or the selected bit is clear.
 */
int
gj_trace_flag_test(uint32_t uFlags, unsigned uBit)
{
	(void)NULL;
	return b1948_flag_test(uFlags, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_trace_flag_test(uint32_t uFlags, unsigned uBit)
    __attribute__((alias("gj_trace_flag_test")));
