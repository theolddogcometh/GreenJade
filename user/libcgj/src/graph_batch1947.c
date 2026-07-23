/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1947: clear one bit in a trace flag word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trace_flag_clear(uint32_t flags, unsigned bit);
 *     — Return flags with bit cleared (LSB is bit 0). Bits outside
 *       0..31 leave flags unchanged (no shift UB). Pure integer;
 *       no builtins, no logger side effects.
 *   uint32_t __gj_trace_flag_clear  (alias)
 *   __libcgj_batch1947_marker = "libcgj-batch1947"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1947_marker[] = "libcgj-batch1947";

/* Trace flag word width (bits). */
#define B1947_FLAG_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

/* Clear bit uBit of uFlags when in range. */
static uint32_t
b1947_flag_clear(uint32_t uFlags, unsigned uBit)
{
	if (uBit >= B1947_FLAG_BITS) {
		return uFlags;
	}
	return uFlags & ~((uint32_t)1u << uBit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trace_flag_clear — clear one bit in a 32-bit trace flag word.
 *
 * flags: current packed trace enable mask
 * bit: bit index (0 = LSB). If bit >= 32, return flags unchanged.
 *
 * Returns flags with the selected bit forced to zero when in range.
 */
uint32_t
gj_trace_flag_clear(uint32_t uFlags, unsigned uBit)
{
	(void)NULL;
	return b1947_flag_clear(uFlags, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trace_flag_clear(uint32_t uFlags, unsigned uBit)
    __attribute__((alias("gj_trace_flag_clear")));
