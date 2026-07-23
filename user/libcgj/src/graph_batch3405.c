/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3405: u32 BLSI (extract lowest set bit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_blsi_u(uint32_t x);
 *     - Return x & -x: isolate the least-significant 1-bit of x, or 0
 *       if x == 0. Pure unsigned two's-complement; no builtins.
 *   uint32_t __gj_u32_blsi_u  (alias)
 *   __libcgj_batch3405_marker = "libcgj-batch3405"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_isolate_lsb (batch420) / gj_isolate_lsb_u64 (batch599) —
 * unique gj_u32_blsi_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3405_marker[] = "libcgj-batch3405";

/* ---- freestanding helpers ---------------------------------------------- */

/* BMI1 BLSI: isolate lowest set bit via x & -x. */
static uint32_t
b3405_blsi(uint32_t uX)
{
	return uX & (uint32_t)(0u - uX);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_blsi_u - isolate the least-significant 1-bit of x.
 *
 * x: unsigned 32-bit value
 *
 * Two's-complement identity: x & -x (unsigned wrap of -x is defined).
 * Examples: 0 → 0; 0x10 → 0x10; 0x12 → 0x2; 0xffffffffu → 1.
 * No parent wires.
 */
uint32_t
gj_u32_blsi_u(uint32_t uX)
{
	(void)NULL;
	return b3405_blsi(uX);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_blsi_u(uint32_t uX)
    __attribute__((alias("gj_u32_blsi_u")));
