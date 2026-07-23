/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2062: freestanding XOR-reduce of a uint32_t
 * array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_xor_n(const uint32_t *a, size_t n);
 *     - Return a[0] ^ a[1] ^ ... ^ a[n-1]. NULL a with n > 0 yields 0;
 *       n == 0 yields 0 (XOR identity). Pure integer fold; no carry.
 *   uint32_t __gj_u32_xor_n  (alias)
 *   __libcgj_batch2062_marker = "libcgj-batch2062"
 *
 * Distinct from gj_u32_xor_all (batch610) - unique gj_u32_xor_n name
 * only; no multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2062_marker[] = "libcgj-batch2062";

/* ---- freestanding helpers ---------------------------------------------- */

/* XOR-fold a[0..cN). Caller guarantees pA non-NULL when cN > 0. */
static uint32_t
b2062_xor(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32X;

	u32X = 0u;
	for (i = 0u; i < cN; i++) {
		u32X ^= pA[i];
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_xor_n - XOR-reduce n uint32_t elements.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 *
 * Returns a[0] ^ ... ^ a[n-1] (0 if a is NULL or n is 0).
 */
uint32_t
gj_u32_xor_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2062_xor(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_xor_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_xor_n")));
