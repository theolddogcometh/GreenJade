/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3013: uint64_t overflow-safe ceiling average
 * (exclusive avg_ceil_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_ceil_u(uint64_t a, uint64_t b);
 *     - Ceiling of (a + b) / 2 without overflow of a + b.
 *   uint64_t __gj_u64_avg_ceil_u  (alias)
 *   __libcgj_batch3013_marker = "libcgj-batch3013"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_u64_avg_floor_u (batch3011), gj_u64_avg_u (batch2644), gj_u64_avg
 * (batch615). Unique surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3013_marker[] = "libcgj-batch3013";

/* ---- freestanding helpers ---------------------------------------------- */

/* Ceiling average via bit ops; no intermediate sum overflow. */
static uint64_t
b3013_avg_ceil(uint64_t u64A, uint64_t u64B)
{
	return (u64A | u64B) - ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_ceil_u - ceil((a + b) / 2) without overflow.
 *
 * Identity: (a | b) - ((a ^ b) >> 1)
 *   Equivalent to floor((a + b + 1) / 2) for all a, b in [0, UINT64_MAX].
 */
uint64_t
gj_u64_avg_ceil_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3013_avg_ceil(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_ceil_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_avg_ceil_u")));
