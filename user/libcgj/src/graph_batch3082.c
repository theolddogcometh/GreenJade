/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3082: uint32_t vector dot product with
 * saturating u64 accumulation (exclusive n_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_dot_u32_n_u(const uint32_t *a, const uint32_t *b, size_t n);
 *     - Return sum_i a[i]*b[i] as uint64_t. Each product widens to u64
 *       exactly (u32*u32 fits). Running sum uses portable sat add.
 *       NULL a or NULL b or n == 0 yields 0.
 *   uint64_t __gj_dot_u32_n_u  (alias)
 *   __libcgj_batch3082_marker = "libcgj-batch3082"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Distinct from
 * gj_u32_dot_n (batch2426) and gj_dot_i32 (batch539) — unique
 * gj_dot_u32_n_u name; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3082_marker[] = "libcgj-batch3082";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b3082_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Exact u32 * u32 product in u64 (always fits). */
static uint64_t
b3082_mul_u32(uint32_t u32A, uint32_t u32B)
{
	return (uint64_t)u32A * (uint64_t)u32B;
}

/*
 * Saturating dot of a[0..cN) with b[0..cN). Caller guarantees both
 * pointers non-NULL when cN > 0.
 */
static uint64_t
b3082_dot(const uint32_t *pA, const uint32_t *pB, size_t cN)
{
	size_t i;
	uint64_t u64Acc;

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc = b3082_sat_add_u64(u64Acc,
		    b3082_mul_u32(pA[i], pB[i]));
		if (u64Acc == UINT64_MAX) {
			return UINT64_MAX;
		}
	}
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dot_u32_n_u - uint32 vector inner product with sat u64 accumulate.
 *
 * a, b: length-n vectors (NULL either -> 0)
 * n:    element count (0 -> 0)
 */
uint64_t
gj_dot_u32_n_u(const uint32_t *a, const uint32_t *b, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL) {
		return 0ull;
	}
	return b3082_dot(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dot_u32_n_u(const uint32_t *a, const uint32_t *b, size_t n)
    __attribute__((alias("gj_dot_u32_n_u")));
