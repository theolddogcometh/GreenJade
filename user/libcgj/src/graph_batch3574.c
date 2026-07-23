/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3574: freestanding bounded prefix equality (_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_prefix_eq_u(const void *hay, size_t n,
 *                          const void *pfx, size_t np);
 *     - Return 1 if hay[0..n) begins with the np octets of pfx, else 0.
 *       np == 0 is a vacuous match (returns 1) even if hay or pfx is
 *       NULL. If np > 0 and either pointer is NULL, or if np > n,
 *       returns 0.
 *   int __gj_mem_prefix_eq_u  (alias)
 *   __libcgj_batch3574_marker = "libcgj-batch3574"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_prefix_eq_u surface only; no multi-def. Distinct from
 * gj_mem_starts_with (batch709) / gj_mem_pref_eq_n (batch2214) /
 * gj_mem_eq_n_u2. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3574_marker[] = "libcgj-batch3574";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if pH[0..np) equals pP[0..np). Caller: both non-NULL, np > 0,
 * and np <= nHay (caller already checked bounds).
 */
static int
b3574_pref_eq(const unsigned char *pH, const unsigned char *pP, size_t np)
{
	size_t i;

	for (i = 0u; i < np; i++) {
		if (pH[i] != pP[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_prefix_eq_u - true if hay[0..n) has pfx[0..np) as a leading window.
 *
 * hay, n:  haystack base and length (NULL with np > 0 -> 0)
 * pfx, np: required prefix base and length (0 -> 1, vacuous match)
 *
 * Does not call libc. Early-exits on the first mismatch.
 * No parent wires.
 */
int
gj_mem_prefix_eq_u(const void *hay, size_t n, const void *pfx, size_t np)
{
	(void)NULL;
	if (np == 0u) {
		return 1;
	}
	if (np > n) {
		return 0;
	}
	if (hay == NULL || pfx == NULL) {
		return 0;
	}
	if (hay == pfx) {
		return 1;
	}
	return b3574_pref_eq((const unsigned char *)hay,
	    (const unsigned char *)pfx, np);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_prefix_eq_u(const void *hay, size_t n, const void *pfx,
    size_t np)
    __attribute__((alias("gj_mem_prefix_eq_u")));
