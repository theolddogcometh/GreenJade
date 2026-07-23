/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3228: uint64_t saturating left shift (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_sat_u(uint64_t x, unsigned n);
 *     - Return x << n for n in 0..63. If n >= 64, return 0 (saturate
 *       the shift count; no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_shl_sat_u  (alias)
 *   __libcgj_batch3228_marker = "libcgj-batch3228"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u64_shl_sat_u surface only; no multi-def. Distinct from
 * gj_u64_shl_sat (batch2405), gj_u32_shl_sat_u (batch3227), and
 * gj_u64_shl_u (batch3222 modular). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3228_marker[] = "libcgj-batch3228";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; n >= 64 saturates to 0. */
static uint64_t
b3228_shl_sat(uint64_t u64X, unsigned uN)
{
if (uN >= 64u) {
return 0u;
}
return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_sat_u - saturating left shift of a 64-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 64 → 0
 *
 * Returns x << n for n in 0..63; otherwise 0. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_u64_shl_sat_u(uint64_t u64X, unsigned uN)
{
(void)NULL;
return b3228_shl_sat(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_sat_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shl_sat_u")));
