/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2029: freestanding pairwise buffer swap.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_swap_bytes(void *a, void *b, size_t n);
 *     — Exchange n octets between regions a and b. Returns n on
 *       success. Any of a, b NULL → return 0 (no write). n == 0 → 0.
 *       Same pointer a == b → return n (no-op exchange).
 *   size_t __gj_mem_swap_bytes  (alias)
 *   __libcgj_batch2029_marker = "libcgj-batch2029"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_swap
 * (batch663) — returns byte count and uses unique gj_mem_swap_bytes
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2029_marker[] = "libcgj-batch2029";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap n octets between pA and pB. Caller: both non-NULL, n > 0, pA != pB. */
static size_t
b2029_swap(unsigned char *pA, unsigned char *pB, size_t n)
{
	size_t i;
	unsigned char uTmp;

	for (i = 0u; i < n; i++) {
		uTmp = pA[i];
		pA[i] = pB[i];
		pB[i] = uTmp;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_swap_bytes — swap n octets between a and b.
 *
 * a: first region of at least n bytes when n > 0 (NULL → 0)
 * b: second region of at least n bytes when n > 0 (NULL → 0)
 * n: number of octets to exchange (0 → 0)
 *
 * Overlap between a and b other than a == b is not supported (per-octet
 * temp walk). When a == b the regions are left unchanged and n is returned.
 */
size_t
gj_mem_swap_bytes(void *pA, void *pB, size_t n)
{
	(void)NULL;
	if (pA == NULL || pB == NULL) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}
	if (pA == pB) {
		return n;
	}
	return b2029_swap((unsigned char *)pA, (unsigned char *)pB, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_swap_bytes(void *pA, void *pB, size_t n)
    __attribute__((alias("gj_mem_swap_bytes")));
