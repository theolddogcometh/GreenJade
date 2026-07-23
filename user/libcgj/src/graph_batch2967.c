/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2967: freestanding pairwise XOR (_n_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_xor_n_u(void *out, const void *a, const void *b,
 *                         size_t n);
 *     - For each i in [0, n): out[i] = a[i] ^ b[i] (unsigned octets).
 *       Returns n on success. Any of out/a/b NULL -> return 0 (no write).
 *       n == 0 -> return 0.
 *   size_t __gj_mem_xor_n_u  (alias)
 *   __libcgj_batch2967_marker = "libcgj-batch2967"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_xor
 * (batch528) / gj_mem_xor_bytes (batch1398) - unique gj_mem_xor_n_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2967_marker[] = "libcgj-batch2967";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pairwise XOR into pOut. Caller: all non-NULL, n > 0. */
static void
b2967_xor(unsigned char *pOut, const unsigned char *pA,
    const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pOut[i] = (unsigned char)(pA[i] ^ pB[i]);
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_xor_n_u - pairwise octet XOR of a and b into out.
 *
 * out: destination of at least n bytes when n > 0 (NULL -> 0)
 * a:   first source region (NULL -> 0)
 * b:   second source region (NULL -> 0)
 * n:   number of octets to XOR (0 -> 0)
 *
 * Overlap between out and a/b is not supported (forward walk only).
 * Returns n when bytes were written, else 0.
 */
size_t
gj_mem_xor_n_u(void *out, const void *a, const void *b, size_t n)
{
	(void)NULL;
	if (out == NULL || a == NULL || b == NULL) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}
	b2967_xor((unsigned char *)out, (const unsigned char *)a,
	    (const unsigned char *)b, n);
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_xor_n_u(void *out, const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_xor_n_u")));
