/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3191: minimum of a uint8_t buffer (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_buf_min_u8_u(const uint8_t *buf, size_t n);
 *     - Return the least value in buf[0..n). buf == NULL or n == 0 → 0.
 *       Soft pure-data buffer min for product path / export bands.
 *   uint8_t __gj_buf_min_u8_u  (alias)
 *   __libcgj_batch3191_marker = "libcgj-batch3191"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_buf_min_u8_u surface only; no multi-def. Distinct from
 * gj_u8_find_min (batch865 index), gj_u32_min_n (batch2065),
 * gj_arr_u32_min_u (batch2864), and gj_u64_min_arr (batch814).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3191_marker[] = "libcgj-batch3191";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least value in pBuf[0..cN). Caller guarantees pBuf non-NULL and cN > 0. */
static uint8_t
b3191_min(const uint8_t *pBuf, size_t cN)
{
size_t i;
uint8_t u8Min;

u8Min = pBuf[0];
for (i = 1u; i < cN; i++) {
if (pBuf[i] < u8Min) {
u8Min = pBuf[i];
}
}
return u8Min;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_min_u8_u - least value in buf[0..n).
 *
 * buf: base (NULL → 0)
 * n:   element count (0 → 0)
 *
 * Does not call libc. No parent wires.
 */
uint8_t
gj_buf_min_u8_u(const uint8_t *pBuf, size_t cN)
{
(void)NULL;
if (pBuf == NULL || cN == 0u) {
return 0u;
}
return b3191_min(pBuf, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_buf_min_u8_u(const uint8_t *pBuf, size_t cN)
    __attribute__((alias("gj_buf_min_u8_u")));
