/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4006: clear the MSB of a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clear_msb_u(uint64_t x);
 *     - Return x with bit 63 cleared (AND with ~0x8000000000000000).
 *       Other bits unchanged. Pure integer; no builtins.
 *   uint64_t __gj_u64_clear_msb_u  (alias)
 *   __libcgj_batch4006_marker = "libcgj-batch4006"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u64_clear_bit (batch2012) and gj_u32_clear_msb_u (batch4005) —
 * unique gj_u64_clear_msb_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4006_marker[] = "libcgj-batch4006";

/* uint64_t MSB mask. */
#define B4006_MSB  ((uint64_t)0x8000000000000000ULL)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4006_clear_msb(uint64_t u64X)
{
	return u64X & ~B4006_MSB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clear_msb_u - clear bit 63 of a 64-bit word.
 *
 * x: value to update
 *
 * Returns x with the MSB forced to 0. Example:
 * gj_u64_clear_msb_u(0x8000000000000000ULL) -> 0.
 * No parent wires.
 */
uint64_t
gj_u64_clear_msb_u(uint64_t u64X)
{
	(void)NULL;
	return b4006_clear_msb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clear_msb_u(uint64_t u64X)
    __attribute__((alias("gj_u64_clear_msb_u")));
