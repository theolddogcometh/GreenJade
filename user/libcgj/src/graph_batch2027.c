/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2027: freestanding octet-wise pairwise OR.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_or_bytes(void *out, const void *a, const void *b,
 *                          size_t n);
 *     — For each i in [0, n): out[i] = a[i] | b[i] (unsigned octets).
 *       Returns n on success. Any of out/a/b NULL → return 0 (no write).
 *       n == 0 → return 0.
 *   size_t __gj_mem_or_bytes  (alias)
 *   __libcgj_batch2027_marker = "libcgj-batch2027"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_or
 * (batch530) / gj_mem_xor_bytes / gj_mem_and_bytes — unique
 * gj_mem_or_bytes surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2027_marker[] = "libcgj-batch2027";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pairwise OR into pOut. Caller: all pointers non-NULL, n > 0. */
static size_t
b2027_or(unsigned char *pOut, const unsigned char *pA,
    const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pOut[i] = (unsigned char)(pA[i] | pB[i]);
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_or_bytes — pairwise octet OR of a and b into out.
 *
 * out: destination of at least n bytes when n > 0 (NULL → 0)
 * a:   first source region (NULL → 0)
 * b:   second source region (NULL → 0)
 * n:   number of octets to process (0 → 0)
 *
 * Overlap between out and a/b is not supported (forward walk only).
 */
size_t
gj_mem_or_bytes(void *pOut, const void *pA, const void *pB, size_t n)
{
	(void)NULL;
	if (pOut == NULL || pA == NULL || pB == NULL) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}
	return b2027_or((unsigned char *)pOut, (const unsigned char *)pA,
	    (const unsigned char *)pB, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_or_bytes(void *pOut, const void *pA, const void *pB,
    size_t n)
    __attribute__((alias("gj_mem_or_bytes")));
