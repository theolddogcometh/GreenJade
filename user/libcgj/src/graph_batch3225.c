/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3225: uint32_t arithmetic right shift (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sar_u(uint32_t x, unsigned n);
 *     - Arithmetic right shift of x as a two's-complement signed
 *       32-bit value, result re-interpreted as uint32_t. Count
 *       reduced mod 32 (n & 31). No shift UB; pure integer; no
 *       signed-shift implementation-defined reliance.
 *   uint32_t __gj_u32_sar_u  (alias)
 *   __libcgj_batch3225_marker = "libcgj-batch3225"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u32_sar_u surface only; no multi-def. Distinct from
 * gj_u64_ashr_sat (batch2409) and gj_u32_shr_u (batch3223 logical).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3225_marker[] = "libcgj-batch3225";

/* Sign bit of a 32-bit two's-complement view. */
#define B3225_SIGN  ((uint32_t)1u << 31)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Arithmetic right shift of u32X as a signed view. Count masked to 5
 * bits. Implemented with unsigned ops only so the result is fully
 * defined (no signed >> reliance).
 */
static uint32_t
b3225_sar(uint32_t u32X, unsigned uN)
{
uN &= 31u;
if (uN == 0u) {
return u32X;
}
if ((u32X & B3225_SIGN) == 0u) {
return u32X >> uN;
}
/* Negative view: logical shift then fill high bits with ones. */
return (u32X >> uN) | (~(uint32_t)0u << (32u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sar_u - arithmetic right shift (signed view, count mod 32).
 *
 * x: bit pattern to shift (MSB is the sign bit)
 * n: shift count; reduced mod 32 (n & 31); 0 → identity
 *
 * Returns the arithmetic-right-shifted bit pattern as uint32_t.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_sar_u(uint32_t u32X, unsigned uN)
{
(void)NULL;
return b3225_sar(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sar_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_sar_u")));
