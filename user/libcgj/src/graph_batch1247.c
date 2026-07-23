/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1247: bitwise XOR-reduce of a uint32_t array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_xor_reduce(const uint32_t *a, size_t n);
 *     — a[0] ^ a[1] ^ ... ^ a[n-1]. n==0 or a==NULL → 0.
 *   uint32_t __gj_u32_xor_reduce  (alias)
 *   __libcgj_batch1247_marker = "libcgj-batch1247"
 *
 * Distinct from gj_u64_xor_reduce — unique u32 surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1247_marker[] = "libcgj-batch1247";

/*
 * gj_u32_xor_reduce — fold array with bitwise XOR (identity 0).
 */
uint32_t
gj_u32_xor_reduce(const uint32_t *pA, size_t cN)
{
	uint32_t u32Acc;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	u32Acc = 0u;
	for (i = 0u; i < cN; i++) {
		u32Acc ^= pA[i];
	}
	return u32Acc;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_xor_reduce(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_xor_reduce")));
