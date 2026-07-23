/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2026: freestanding all-equal-to-byte predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_eq_byte_n(const void *p, size_t n, int c);
 *     — Return 1 if every octet in p[0..n) equals (unsigned char)c,
 *       else 0. n == 0 is vacuously true (returns 1) even if p is NULL.
 *       If n > 0 and p is NULL, returns 0.
 *   int __gj_mem_eq_byte_n  (alias)
 *   __libcgj_batch2026_marker = "libcgj-batch2026"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_eq /
 * gj_mem_eq_n / gj_mem_is_zero_n / gj_mem_is_ff_n — unique
 * gj_mem_eq_byte_n surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2026_marker[] = "libcgj-batch2026";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pU[0..n) is all uNeedle. Caller: pU non-NULL when n>0. */
static int
b2026_eq_byte(const unsigned char *pU, size_t n, unsigned char uNeedle)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] != uNeedle) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_eq_byte_n — true if p[0..n) is all equal to (unsigned char)c.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 1, empty window is vacuously equal)
 * c: target byte value (cast to unsigned char)
 *
 * Does not call libc. Early-exits on the first mismatch.
 */
int
gj_mem_eq_byte_n(const void *p, size_t n, int c)
{
	unsigned char uNeedle;

	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (p == NULL) {
		return 0;
	}
	uNeedle = (unsigned char)c;
	return b2026_eq_byte((const unsigned char *)p, n, uNeedle);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_eq_byte_n(const void *p, size_t n, int c)
    __attribute__((alias("gj_mem_eq_byte_n")));
