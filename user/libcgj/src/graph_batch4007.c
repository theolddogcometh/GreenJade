/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4007: set the MSB of a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_set_msb_u(uint32_t x);
 *     - Return x with bit 31 set (OR with 0x80000000). Other bits
 *       unchanged. Pure integer; no builtins.
 *   uint32_t __gj_u32_set_msb_u  (alias)
 *   __libcgj_batch4007_marker = "libcgj-batch4007"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u32_set_bit (batch2017) — unique gj_u32_set_msb_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4007_marker[] = "libcgj-batch4007";

/* uint32_t MSB mask. */
#define B4007_MSB  ((uint32_t)0x80000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4007_set_msb(uint32_t u32X)
{
	return u32X | B4007_MSB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_set_msb_u - set bit 31 of a 32-bit word.
 *
 * x: value to update
 *
 * Returns x | 0x80000000u. Example: gj_u32_set_msb_u(0) -> 0x80000000u;
 * gj_u32_set_msb_u(1u) -> 0x80000001u.
 * No parent wires.
 */
uint32_t
gj_u32_set_msb_u(uint32_t u32X)
{
	(void)NULL;
	return b4007_set_msb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_set_msb_u(uint32_t u32X)
    __attribute__((alias("gj_u32_set_msb_u")));
