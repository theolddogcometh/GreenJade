/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3408: u64 BLSR (reset lowest set bit).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_blsr_u(uint64_t x);
 *     - Return x & (x - 1): clear the least-significant 1-bit of x.
 *       Pure unsigned arithmetic; no builtins.
 *   uint64_t __gj_u64_blsr_u  (alias)
 *   __libcgj_batch3408_marker = "libcgj-batch3408"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_u64_blsi_u (batch3406) / gj_u32_blsr_u (batch3407) —
 * unique gj_u64_blsr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3408_marker[] = "libcgj-batch3408";

/* ---- freestanding helpers ---------------------------------------------- */

/* BMI1 BLSR: reset lowest set bit via x & (x - 1). */
static uint64_t
b3408_blsr(uint64_t uX)
{
	return uX & (uX - 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_blsr_u - clear the least-significant 1-bit of x.
 *
 * x: unsigned 64-bit value
 *
 * Identity: x & (x - 1). Examples: 0 → 0; 0x10 → 0; 0x12 → 0x10;
 * 0xffffffffffffffff → 0xfffffffffffffffe. No parent wires.
 */
uint64_t
gj_u64_blsr_u(uint64_t uX)
{
	(void)NULL;
	return b3408_blsr(uX);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_blsr_u(uint64_t uX)
    __attribute__((alias("gj_u64_blsr_u")));
