/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1379: uint64_t absolute difference.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_absdiff(uint64_t a, uint64_t b);
 *     — |a - b| as uint64_t (always non-negative; no wrap).
 *   uint64_t __gj_u64_absdiff  (alias)
 *   __libcgj_batch1379_marker = "libcgj-batch1379"
 *
 * Unique surface only; no multi-def. Distinct from static helpers in
 * other TUs (e.g. b232_absdiff) and from gj_u32_absdiff (batch1378).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1379_marker[] = "libcgj-batch1379";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1379_absdiff_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A >= u64B) ? (u64A - u64B) : (u64B - u64A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_absdiff — absolute difference of two uint64_t values.
 */
uint64_t
gj_u64_absdiff(uint64_t u64A, uint64_t u64B)
{
	return b1379_absdiff_u64(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_absdiff(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_absdiff")));
