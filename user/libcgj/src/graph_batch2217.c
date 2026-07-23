/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2217: freestanding forward octet copy.
 *
 * Surface (unique symbols):
 *   void *gj_mem_copy_fwd(void *d, const void *s, size_t n);
 *     - Copy n octets from s to d walking low-to-high (forward).
 *       Returns d. d == NULL or s == NULL -> return d (no write when
 *       d is NULL). n == 0 -> return d unchanged.
 *       Regions must not overlap, or d <= s for safe overlap use;
 *       for d > s overlapping regions use gj_mem_copy_bwd.
 *   void *__gj_mem_copy_fwd  (alias)
 *   __libcgj_batch2217_marker = "libcgj-batch2217"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from gj_mem_cpy
 * (batch397) / gj_mem_move - unique gj_mem_copy_fwd surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2217_marker[] = "libcgj-batch2217";

/* ---- freestanding helpers ---------------------------------------------- */

/* Forward copy n octets. Caller: both non-NULL, n > 0. */
static void
b2217_copy_fwd(unsigned char *pD, const unsigned char *pS, size_t n)
{
	size_t iByte;

	for (iByte = 0u; iByte < n; iByte++) {
		pD[iByte] = pS[iByte];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_copy_fwd - copy n octets from s to d (forward walk).
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to copy
 *
 * Does not call libc memcpy. Safe when d <= s (including non-overlap
 * and overlapping with dest not above source). Use gj_mem_copy_bwd
 * when d > s and regions may overlap.
 */
void *
gj_mem_copy_fwd(void *d, const void *s, size_t n)
{
	(void)NULL;
	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u) {
		return d;
	}
	b2217_copy_fwd((unsigned char *)d, (const unsigned char *)s, n);
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_copy_fwd(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_copy_fwd")));
