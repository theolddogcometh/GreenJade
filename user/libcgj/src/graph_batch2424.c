/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2424: floor average of a uint64_t array
 * (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_floor_n(const uint64_t *a, size_t n);
 *     - Return floor((a[0] + ... + a[n-1]) / n). NULL a or n == 0
 *       yields 0. Partial sum saturates at UINT64_MAX.
 *   uint64_t __gj_u64_avg_floor_n  (alias)
 *   __libcgj_batch2424_marker = "libcgj-batch2424"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from
 * gj_u32_avg_floor_n (batch2423) - 64-bit element surface only; no
 * multi-def. Portable sat add only; no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2424_marker[] = "libcgj-batch2424";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2424_sat_add(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * floor(sum(a[0..cN)) / cN) with sat sum. Caller guarantees pA
 * non-NULL and cN > 0.
 */
static uint64_t
b2424_avg_floor(const uint64_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;

	u64Sum = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Sum = b2424_sat_add(u64Sum, pA[i]);
	}
	return u64Sum / (uint64_t)cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_floor_n - floor average of n uint64_t elements.
 *
 * a: base (NULL -> 0)
 * n: element count (0 -> 0)
 *
 * Returns floor(sum / n). Sum saturates at UINT64_MAX before divide.
 */
uint64_t
gj_u64_avg_floor_n(const uint64_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b2424_avg_floor(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_floor_n(const uint64_t *a, size_t n)
    __attribute__((alias("gj_u64_avg_floor_n")));
