/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3215: bitwise OR of two uint64_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_or_u(uint64_t a, uint64_t b);
 *     - Return a | b. Pure integer; no builtins.
 *   uint64_t __gj_u64_or_u  (alias)
 *   __libcgj_batch3215_marker = "libcgj-batch3215"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u64_or_u surface only; no multi-def. Distinct from
 * gj_u64_or_reduce (batch1242) / gj_bitmap_or_u (batch2858) /
 * gj_u32_or_u (batch3214). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3215_marker[] = "libcgj-batch3215";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise OR of two 64-bit words. */
static uint64_t
b3215_or(uint64_t u64A, uint64_t u64B)
{
return u64A | u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_or_u - bitwise OR of two uint64_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns a | b. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_or_u(uint64_t u64A, uint64_t u64B)
{
(void)NULL;
return b3215_or(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_or_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_or_u")));
