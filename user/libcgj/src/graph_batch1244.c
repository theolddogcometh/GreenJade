/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1244: bitwise XOR-reduce of a uint64_t array.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_xor_reduce(const uint64_t *a, size_t n);
 *     — a[0] ^ a[1] ^ ... ^ a[n-1]. n==0 or a==NULL → 0.
 *   uint64_t __gj_u64_xor_reduce  (alias)
 *   __libcgj_batch1244_marker = "libcgj-batch1244"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1244_marker[] = "libcgj-batch1244";

/*
 * gj_u64_xor_reduce — fold array with bitwise XOR (identity 0).
 */
uint64_t
gj_u64_xor_reduce(const uint64_t *pA, size_t cN)
{
	uint64_t u64Acc;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0ull;
	}

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc ^= pA[i];
	}
	return u64Acc;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_xor_reduce(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_u64_xor_reduce")));
