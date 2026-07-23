/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2022: freestanding all-0xFF n-byte predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_is_ff_n(const void *p, size_t n);
 *     — Return 1 if every octet in p[0..n) is 0xFF, else 0. n == 0 is
 *       vacuously all-FF (returns 1) even if p is NULL. If n > 0 and
 *       p is NULL, returns 0.
 *   int __gj_mem_is_ff_n  (alias)
 *   __libcgj_batch2022_marker = "libcgj-batch2022"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_fill_ff
 * (batch804) / gj_mem_is_zero_n (batch2021) — unique gj_mem_is_ff_n
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2022_marker[] = "libcgj-batch2022";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if p[0..n) is all 0xFF octets. Caller guarantees p non-NULL when n>0. */
static int
b2022_is_ff(const unsigned char *pU, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] != 0xFFu) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_is_ff_n — true if p[0..n) is all 0xFF octets.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 1, empty window is vacuously FF)
 *
 * Does not call libc. Early-exits on the first non-0xFF byte.
 */
int
gj_mem_is_ff_n(const void *p, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (p == NULL) {
		return 0;
	}
	return b2022_is_ff((const unsigned char *)p, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_is_ff_n(const void *p, size_t n)
    __attribute__((alias("gj_mem_is_ff_n")));
