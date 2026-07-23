/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3229: extract a bit-field from a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_extract_bits_u(uint32_t x, unsigned lo, unsigned len);
 *     - Extract len consecutive bits starting at bit index lo (0-based
 *       from the LSB). Result is right-aligned in the low bits of the
 *       return value. len == 0 or lo >= 32 yields 0. If lo + len would
 *       run past bit 31, only the remaining high bits of x are taken
 *       (no undefined shift).
 *   uint32_t __gj_u32_extract_bits_u  (alias)
 *   __libcgj_batch3229_marker = "libcgj-batch3229"
 *
 * Milestone 3230 exclusive continuum CREATE-ONLY (3221-3230). Unique
 * gj_u32_extract_bits_u surface only; no multi-def. Distinct from
 * gj_extract_bits_u64 (batch593) and gj_u64_extract_bits (batch1389).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3229_marker[] = "libcgj-batch3229";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Mask of the low uW bits of a u32. uW must be in 1..31; caller handles
 * the full-width (32) and empty (0) cases separately to avoid (1<<32).
 */
static uint32_t
b3229_mask_lo(unsigned uW)
{
return (1u << uW) - 1u;
}

/*
 * Right-aligned bit-field extract over [lo, lo+len). Caps len against
 * the remaining width above lo. Never shifts by 32.
 */
static uint32_t
b3229_extract(uint32_t u32X, unsigned uLo, unsigned uLen)
{
unsigned uRem;

if (uLen == 0u || uLo >= 32u) {
return 0u;
}

uRem = 32u - uLo;
if (uLen > uRem) {
uLen = uRem;
}

u32X >>= uLo;
if (uLen == 32u) {
/* lo was 0 and full width requested — whole word */
return u32X;
}

return u32X & b3229_mask_lo(uLen);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_extract_bits_u - right-aligned bit-field extract (u32).
 *
 * x:   source word
 * lo:  starting bit index (0 = LSB)
 * len: number of bits to extract
 *
 * Shift and mask carefully: never shift a uint32_t by 32, and never
 * form (1 << 32). Cap len against the remaining width above lo.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_extract_bits_u(uint32_t u32X, unsigned uLo, unsigned uLen)
{
(void)NULL;
return b3229_extract(u32X, uLo, uLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_extract_bits_u(uint32_t u32X, unsigned uLo, unsigned uLen)
    __attribute__((alias("gj_u32_extract_bits_u")));
