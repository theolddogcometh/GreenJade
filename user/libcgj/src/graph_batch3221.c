/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3221: uint32_t left shift (_u continuum).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shl_u(uint32_t x, unsigned n);
 *     - Return x << (n & 31). Count reduced mod 32 (no shift UB).
 *       Pure integer; no builtins.
 *   uint32_t __gj_u32_shl_u  (alias)
 *   __libcgj_batch3221_marker = "libcgj-batch3221"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u32_shl_u surface only; no multi-def. Distinct from
 * gj_u32_shl_sat (batch2407) and gj_u32_shl_sat_u (batch3227).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3221_marker[] = "libcgj-batch3221";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; count masked to 5 bits. */
static uint32_t
b3221_shl(uint32_t u32X, unsigned uN)
{
uN &= 31u;
if (uN == 0u) {
return u32X;
}
return u32X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shl_u - left shift a 32-bit word (count mod 32).
 *
 * x: value to shift
 * n: shift count; reduced mod 32 (n & 31); 0 → identity
 *
 * Returns x << (n & 31). Does not call libc. No parent wires.
 */
uint32_t
gj_u32_shl_u(uint32_t u32X, unsigned uN)
{
(void)NULL;
return b3221_shl(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shl_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shl_u")));
