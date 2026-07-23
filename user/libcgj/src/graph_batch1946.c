/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1946: set one bit in a trace flag word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trace_flag_set(uint32_t flags, unsigned bit);
 *     — Return flags with bit set (bit in 0..31, LSB = bit 0).
 *       If bit >= 32, return flags unchanged. Pure integer OR;
 *       no builtins, no logger side effects.
 *   uint32_t __gj_trace_flag_set  (alias)
 *   __libcgj_batch1946_marker = "libcgj-batch1946"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1946_marker[] = "libcgj-batch1946";

/* Trace flag word width (bits). */
#define B1946_FLAG_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

/* Set bit uBit of uFlags when in range. */
static uint32_t
b1946_flag_set(uint32_t uFlags, unsigned uBit)
{
	if (uBit >= B1946_FLAG_BITS) {
		return uFlags;
	}
	return uFlags | ((uint32_t)1u << uBit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trace_flag_set — set one bit in a 32-bit trace flag word.
 *
 * flags: current packed trace enable mask
 * bit: bit index; 0 is LSB. bit >= 32 leaves flags unchanged.
 *
 * Returns flags | (1u << bit) for bit in 0..31; otherwise flags.
 */
uint32_t
gj_trace_flag_set(uint32_t uFlags, unsigned uBit)
{
	(void)NULL;
	return b1946_flag_set(uFlags, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trace_flag_set(uint32_t uFlags, unsigned uBit)
    __attribute__((alias("gj_trace_flag_set")));
