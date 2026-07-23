/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3084: uint32_t squared L2 norm (sum of
 * squares) with saturating u64 accumulation (exclusive n_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_norm2_u32_n_u(const uint32_t *a, size_t n);
 *     - Return sum_i a[i]*a[i] as uint64_t. Each square widens to u64
 *       exactly (u32*u32 fits). Running sum uses portable sat add.
 *       NULL a or n == 0 yields 0.
 *   uint64_t __gj_norm2_u32_n_u  (alias)
 *   __libcgj_batch3084_marker = "libcgj-batch3084"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Distinct from
 * gj_u32_variance_hint_n (batch2425 floor sumsq/n) and gj_l2sq_i32
 * (batch540 pairwise) — unique gj_norm2_u32_n_u name; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3084_marker[] = "libcgj-batch3084";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b3084_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Exact u32 square in u64 (always fits). */
static uint64_t
b3084_sq_u32(uint32_t u32V)
{
	return (uint64_t)u32V * (uint64_t)u32V;
}

/*
 * Saturating sum of squares of a[0..cN). Caller guarantees pA non-NULL
 * when cN > 0.
 */
static uint64_t
b3084_norm2(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Acc;

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc = b3084_sat_add_u64(u64Acc, b3084_sq_u32(pA[i]));
		if (u64Acc == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_norm2_u32_n_u - uint32 squared L2 norm with sat u64 accumulate.
 *
 * a: length-n vector (NULL -> 0)
 * n: element count (0 -> 0)
 */
uint64_t
gj_norm2_u32_n_u(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b3084_norm2(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_norm2_u32_n_u(const uint32_t *a, size_t n)
    __attribute__((alias("gj_norm2_u32_n_u")));
