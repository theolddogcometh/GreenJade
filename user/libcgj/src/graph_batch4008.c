/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4008: set the MSB of a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_set_msb_u(uint64_t x);
 *     - Return x with bit 63 set (OR with 0x8000000000000000). Other
 *       bits unchanged. Pure integer; no builtins.
 *   uint64_t __gj_u64_set_msb_u  (alias)
 *   __libcgj_batch4008_marker = "libcgj-batch4008"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u64_set_bit (batch2011) and gj_u32_set_msb_u (batch4007) —
 * unique gj_u64_set_msb_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4008_marker[] = "libcgj-batch4008";

/* uint64_t MSB mask. */
#define B4008_MSB  ((uint64_t)0x8000000000000000ULL)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4008_set_msb(uint64_t u64X)
{
	return u64X | B4008_MSB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_set_msb_u - set bit 63 of a 64-bit word.
 *
 * x: value to update
 *
 * Returns x | 0x8000000000000000ULL. Example:
 * gj_u64_set_msb_u(0) -> 0x8000000000000000ULL.
 * No parent wires.
 */
uint64_t
gj_u64_set_msb_u(uint64_t u64X)
{
	(void)NULL;
	return b4008_set_msb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_set_msb_u(uint64_t u64X)
    __attribute__((alias("gj_u64_set_msb_u")));
