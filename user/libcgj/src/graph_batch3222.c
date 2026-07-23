/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3222: uint64_t left shift (_u continuum).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_u(uint64_t x, unsigned n);
 *     - Return x << (n & 63). Count reduced mod 64 (no shift UB).
 *       Pure integer; no builtins.
 *   uint64_t __gj_u64_shl_u  (alias)
 *   __libcgj_batch3222_marker = "libcgj-batch3222"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u64_shl_u surface only; no multi-def. Distinct from
 * gj_u64_shl_sat (batch2405), gj_u32_shl_u (batch3221), and
 * gj_u64_shl_sat_u (batch3228). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3222_marker[] = "libcgj-batch3222";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; count masked to 6 bits. */
static uint64_t
b3222_shl(uint64_t u64X, unsigned uN)
{
uN &= 63u;
if (uN == 0u) {
return u64X;
}
return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_u - left shift a 64-bit word (count mod 64).
 *
 * x: value to shift
 * n: shift count; reduced mod 64 (n & 63); 0 → identity
 *
 * Returns x << (n & 63). Does not call libc. No parent wires.
 */
uint64_t
gj_u64_shl_u(uint64_t u64X, unsigned uN)
{
(void)NULL;
return b3222_shl(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shl_u")));
