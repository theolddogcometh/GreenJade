/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4005: clear the MSB of a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clear_msb_u(uint32_t x);
 *     - Return x with bit 31 cleared (AND with ~0x80000000). Other bits
 *       unchanged. Pure integer; no builtins.
 *   uint32_t __gj_u32_clear_msb_u  (alias)
 *   __libcgj_batch4005_marker = "libcgj-batch4005"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u32_clear_bit (batch2018) — unique gj_u32_clear_msb_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4005_marker[] = "libcgj-batch4005";

/* uint32_t MSB mask. */
#define B4005_MSB  ((uint32_t)0x80000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4005_clear_msb(uint32_t u32X)
{
	return u32X & ~B4005_MSB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clear_msb_u - clear bit 31 of a 32-bit word.
 *
 * x: value to update
 *
 * Returns x with the MSB forced to 0. Example:
 * gj_u32_clear_msb_u(0x80000000u) -> 0; gj_u32_clear_msb_u(0xFFu) -> 0xFFu.
 * No parent wires.
 */
uint32_t
gj_u32_clear_msb_u(uint32_t u32X)
{
	(void)NULL;
	return b4005_clear_msb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clear_msb_u(uint32_t u32X)
    __attribute__((alias("gj_u32_clear_msb_u")));
