/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3223: uint32_t logical right shift (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shr_u(uint32_t x, unsigned n);
 *     - Return x >> (n & 31) (logical). Count reduced mod 32 (no
 *       shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_shr_u  (alias)
 *   __libcgj_batch3223_marker = "libcgj-batch3223"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u32_shr_u surface only; no multi-def. Distinct from
 * gj_u32_shr_sat (batch2408) and gj_u32_shl_u (batch3221).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3223_marker[] = "libcgj-batch3223";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical right shift; count masked to 5 bits. */
static uint32_t
b3223_shr(uint32_t u32X, unsigned uN)
{
uN &= 31u;
if (uN == 0u) {
return u32X;
}
return u32X >> uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shr_u - logical right shift a 32-bit word (count mod 32).
 *
 * x: value to shift
 * n: shift count; reduced mod 32 (n & 31); 0 → identity
 *
 * Returns x >> (n & 31). Does not call libc. No parent wires.
 */
uint32_t
gj_u32_shr_u(uint32_t u32X, unsigned uN)
{
(void)NULL;
return b3223_shr(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shr_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shr_u")));
