/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3085: uint64_t squared L2 norm (sum of
 * squares) with saturating u64 product and accumulation (exclusive n_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_norm2_u64_n_u(const uint64_t *a, size_t n);
 *     - Return sum_i sat_mul(a[i], a[i]) as uint64_t with sat add
 *       accumulate. Software path only (no wide mul / __int128).
 *       NULL a or n == 0 yields 0.
 *   uint64_t __gj_norm2_u64_n_u  (alias)
 *   __libcgj_batch3085_marker = "libcgj-batch3085"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Distinct from
 * gj_norm2_u32_n_u (batch3084) — unique gj_norm2_u64_n_u name; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3085_marker[] = "libcgj-batch3085";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b3085_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * Saturating u64 * u64: overflow iff a > floor(UINT64_MAX / b) when both
 * factors are non-zero. No __int128 / wide mul.
 */
static uint64_t
b3085_sat_mul_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/*
 * Saturating sum of squares of a[0..cN). Caller guarantees pA non-NULL
 * when cN > 0.
 */
static uint64_t
b3085_norm2(const uint64_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Acc;

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc = b3085_sat_add_u64(u64Acc,
		    b3085_sat_mul_u64(pA[i], pA[i]));
		if (u64Acc == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_norm2_u64_n_u - uint64 squared L2 norm with sat u64 mul/add.
 *
 * a: length-n vector (NULL -> 0)
 * n: element count (0 -> 0)
 */
uint64_t
gj_norm2_u64_n_u(const uint64_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b3085_norm2(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_norm2_u64_n_u(const uint64_t *a, size_t n)
    __attribute__((alias("gj_norm2_u64_n_u")));
