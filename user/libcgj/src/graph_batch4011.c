/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4011: toggle the MSB of a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_toggle_msb_u(uint64_t x);
 *     - Flip bit 63 of x (XOR with 0x8000000000000000). Other bits
 *       unchanged. Pure integer; no builtins.
 *   uint64_t __gj_u64_toggle_msb_u  (alias)
 *   __libcgj_batch4011_marker = "libcgj-batch4011"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020: u64_toggle_msb_u,
 * u32_mask_below_u, u64_mask_below_u, u32_mask_above_u,
 * u64_mask_above_u, u32_isolate_msb_u, u64_isolate_msb_u,
 * u32_isolate_lsb_u, u64_isolate_lsb_u, batch_id_4020). Distinct from
 * gj_u32_toggle_msb_u (batch4009), gj_u64_toggle_bit (batch2013), and
 * gj_u64_toggle_bits_u (batch3239) — unique gj_u64_toggle_msb_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4011_marker[] = "libcgj-batch4011";

/* uint64_t MSB mask (bit 63). */
#define B4011_MSB  ((uint64_t)0x8000000000000000ULL)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4011_toggle_msb(uint64_t u64X)
{
	return u64X ^ B4011_MSB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_toggle_msb_u - flip bit 63 of a 64-bit word.
 *
 * x: value whose MSB is to be toggled
 *
 * Returns x ^ 0x8000000000000000ULL. Example:
 * gj_u64_toggle_msb_u(0) -> 0x8000000000000000ULL;
 * gj_u64_toggle_msb_u(0x8000000000000000ULL) -> 0.
 * No parent wires.
 */
uint64_t
gj_u64_toggle_msb_u(uint64_t u64X)
{
	(void)NULL;
	return b4011_toggle_msb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_toggle_msb_u(uint64_t u64X)
    __attribute__((alias("gj_u64_toggle_msb_u")));
