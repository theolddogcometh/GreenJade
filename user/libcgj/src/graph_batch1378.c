/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1378: uint32_t absolute difference.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_absdiff(uint32_t a, uint32_t b);
 *     — |a - b| as uint32_t (always non-negative; no wrap).
 *   uint32_t __gj_u32_absdiff  (alias)
 *   __libcgj_batch1378_marker = "libcgj-batch1378"
 *
 * Unique surface only; no multi-def. Distinct from static helpers in
 * other TUs (e.g. b232_absdiff).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1378_marker[] = "libcgj-batch1378";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1378_absdiff_u32(uint32_t u32A, uint32_t u32B)
{
	return (u32A >= u32B) ? (u32A - u32B) : (u32B - u32A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_absdiff — absolute difference of two uint32_t values.
 */
uint32_t
gj_u32_absdiff(uint32_t u32A, uint32_t u32B)
{
	return b1378_absdiff_u32(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_absdiff(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_absdiff")));
