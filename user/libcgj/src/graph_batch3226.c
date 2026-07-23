/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3226: uint64_t arithmetic right shift (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sar_u(uint64_t x, unsigned n);
 *     - Arithmetic right shift of x as a two's-complement signed
 *       64-bit value, result re-interpreted as uint64_t. Count
 *       reduced mod 64 (n & 63). No shift UB; pure integer; no
 *       signed-shift implementation-defined reliance.
 *   uint64_t __gj_u64_sar_u  (alias)
 *   __libcgj_batch3226_marker = "libcgj-batch3226"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u64_sar_u surface only; no multi-def. Distinct from
 * gj_u64_ashr_sat (batch2409), gj_u32_sar_u (batch3225), and
 * gj_u64_shr_u (batch3224 logical). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3226_marker[] = "libcgj-batch3226";

/* Sign bit of a 64-bit two's-complement view. */
#define B3226_SIGN  ((uint64_t)1 << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Arithmetic right shift of u64X as a signed view. Count masked to 6
 * bits. Implemented with unsigned ops only so the result is fully
 * defined (no signed >> reliance).
 */
static uint64_t
b3226_sar(uint64_t u64X, unsigned uN)
{
uN &= 63u;
if (uN == 0u) {
return u64X;
}
if ((u64X & B3226_SIGN) == 0u) {
return u64X >> uN;
}
/* Negative view: logical shift then fill high bits with ones. */
return (u64X >> uN) | (~(uint64_t)0 << (64u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sar_u - arithmetic right shift (signed view, count mod 64).
 *
 * x: bit pattern to shift (MSB is the sign bit)
 * n: shift count; reduced mod 64 (n & 63); 0 → identity
 *
 * Returns the arithmetic-right-shifted bit pattern as uint64_t.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_sar_u(uint64_t u64X, unsigned uN)
{
(void)NULL;
return b3226_sar(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sar_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_sar_u")));
