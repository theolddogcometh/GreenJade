/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3406: u64 BLSI (extract lowest set bit).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_blsi_u(uint64_t x);
 *     - Return x & -x: isolate the least-significant 1-bit of x, or 0
 *       if x == 0. Pure unsigned two's-complement; no builtins.
 *   uint64_t __gj_u64_blsi_u  (alias)
 *   __libcgj_batch3406_marker = "libcgj-batch3406"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_isolate_lsb (batch420) / gj_u32_blsi_u (batch3405) —
 * unique gj_u64_blsi_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3406_marker[] = "libcgj-batch3406";

/* ---- freestanding helpers ---------------------------------------------- */

/* BMI1 BLSI: isolate lowest set bit via x & -x. */
static uint64_t
b3406_blsi(uint64_t uX)
{
	return uX & (uint64_t)(0ull - uX);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_blsi_u - isolate the least-significant 1-bit of x.
 *
 * x: unsigned 64-bit value
 *
 * Two's-complement identity: x & -x (unsigned wrap of -x is defined).
 * Examples: 0 → 0; 0x10 → 0x10; 0x12 → 0x2; 0xffffffffffffffff → 1.
 * No parent wires.
 */
uint64_t
gj_u64_blsi_u(uint64_t uX)
{
	(void)NULL;
	return b3406_blsi(uX);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_blsi_u(uint64_t uX)
    __attribute__((alias("gj_u64_blsi_u")));
