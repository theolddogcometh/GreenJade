/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch397: freestanding memory copy (non-overlapping).
 *
 * Surface (unique symbols):
 *   void *gj_mem_cpy(void *d, const void *s, size_t n);
 *     — Copy n octets from s to d (forward byte copy). Returns d.
 *       d == NULL or s == NULL → return d (no write when d is NULL).
 *       n == 0 → return d unchanged.
 *       Regions must not overlap; for overlap use gj_mem_move.
 *   void *__gj_mem_cpy  (alias)
 *   __libcgj_batch397_marker = "libcgj-batch397"
 *
 * Distinct from libc memcpy in string.c — unique gj_ names only; no
 * multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch397_marker[] = "libcgj-batch397";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_cpy — copy n octets from s to d (forward).
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to copy
 *
 * Does not call libc memcpy. Overlapping regions are undefined for this
 * routine; use gj_mem_move when regions may overlap.
 */
void *
gj_mem_cpy(void *d, const void *s, size_t n)
{
	unsigned char *pd;
	const unsigned char *ps;
	size_t i;

	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u) {
		return d;
	}

	pd = (unsigned char *)d;
	ps = (const unsigned char *)s;
	for (i = 0u; i < n; i++) {
		pd[i] = ps[i];
	}
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_cpy(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_cpy")));
