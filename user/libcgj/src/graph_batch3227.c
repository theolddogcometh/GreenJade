/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3227: uint32_t saturating left shift (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shl_sat_u(uint32_t x, unsigned n);
 *     - Return x << n for n in 0..31. If n >= 32, return 0 (saturate
 *       the shift count; no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_shl_sat_u  (alias)
 *   __libcgj_batch3227_marker = "libcgj-batch3227"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u32_shl_sat_u surface only; no multi-def. Distinct from
 * gj_u32_shl_sat (batch2407) and gj_u32_shl_u (batch3221 modular).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3227_marker[] = "libcgj-batch3227";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; n >= 32 saturates to 0. */
static uint32_t
b3227_shl_sat(uint32_t u32X, unsigned uN)
{
if (uN >= 32u) {
return 0u;
}
return u32X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shl_sat_u - saturating left shift of a 32-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 32 → 0
 *
 * Returns x << n for n in 0..31; otherwise 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_u32_shl_sat_u(uint32_t u32X, unsigned uN)
{
(void)NULL;
return b3227_shl_sat(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shl_sat_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shl_sat_u")));
