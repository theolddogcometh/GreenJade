/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1399: octet-wise AND of two buffers into out.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_and_bytes(void *out, const void *a, const void *b,
 *                           size_t n);
 *     — For each i in [0, n): out[i] = a[i] & b[i] (unsigned octets).
 *       Returns n on success. Any of out/a/b NULL → return 0 (no write).
 *       n == 0 → return 0.
 *   size_t __gj_mem_and_bytes  (alias)
 *   __libcgj_batch1399_marker = "libcgj-batch1399"
 *
 * Distinct from gj_mem_and (batch529) — returns byte count and uses the
 * unique gj_mem_and_bytes name. Does NOT redefine prior symbols.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1399_marker[] = "libcgj-batch1399";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_and_bytes — pairwise octet AND of a and b into out.
 *
 * Reads a[i]/b[i] then writes out[i] in ascending i order.
 */
size_t
gj_mem_and_bytes(void *pOut, const void *pA, const void *pB, size_t n)
{
	unsigned char *pd;
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (pOut == NULL || pA == NULL || pB == NULL) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}

	pd = (unsigned char *)pOut;
	pa = (const unsigned char *)pA;
	pb = (const unsigned char *)pB;
	for (i = 0u; i < n; i++) {
		pd[i] = (unsigned char)(pa[i] & pb[i]);
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_and_bytes(void *pOut, const void *pA, const void *pB,
    size_t n)
    __attribute__((alias("gj_mem_and_bytes")));
