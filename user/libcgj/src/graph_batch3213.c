/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3213: bitwise AND of two uint64_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_and_u(uint64_t a, uint64_t b);
 *     - Return a & b. Pure integer; no builtins.
 *   uint64_t __gj_u64_and_u  (alias)
 *   __libcgj_batch3213_marker = "libcgj-batch3213"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u64_and_u surface only; no multi-def. Distinct from
 * gj_bitmap_and_u (batch2859) / gj_u32_and_u (batch3212). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3213_marker[] = "libcgj-batch3213";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise AND of two 64-bit words. */
static uint64_t
b3213_and(uint64_t u64A, uint64_t u64B)
{
return u64A & u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_and_u - bitwise AND of two uint64_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns a & b. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_and_u(uint64_t u64A, uint64_t u64B)
{
(void)NULL;
return b3213_and(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_and_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_and_u")));
