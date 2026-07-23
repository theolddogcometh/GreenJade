/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1227: naive LCP of two explicit-length strings.
 *
 * Surface (unique symbols):
 *   size_t gj_lcp_naive(const char *a, size_t na, const char *b, size_t nb);
 *     — Number of leading bytes equal between a[0..na) and b[0..nb).
 *       Stops at first mismatch or either length. NULL either pointer
 *       with positive length → 0. Distinct from NUL-terminated
 *       gj_lcp_len (batch237).
 *   size_t __gj_lcp_naive  (alias)
 *   __libcgj_batch1227_marker = "libcgj-batch1227"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1227_marker[] = "libcgj-batch1227";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcp_naive — longest common prefix length of two bounded buffers.
 */
size_t
gj_lcp_naive(const char *szA, size_t cNa, const char *szB, size_t cNb)
{
	size_t cLim;
	size_t i;

	if (cNa == 0u || cNb == 0u) {
		return 0u;
	}
	if (szA == NULL || szB == NULL) {
		return 0u;
	}

	cLim = (cNa < cNb) ? cNa : cNb;
	i = 0u;
	while (i < cLim &&
	       (unsigned char)szA[i] == (unsigned char)szB[i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lcp_naive(const char *szA, size_t cNa, const char *szB, size_t cNb)
    __attribute__((alias("gj_lcp_naive")));
