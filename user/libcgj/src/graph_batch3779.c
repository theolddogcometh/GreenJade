/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3779: Z-algorithm Z-box step (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_z_box_u(const uint8_t *s, size_t n, size_t i,
 *                     size_t *p_l, size_t *p_r, const size_t *z);
 *     - Compute Z[i] for s[0..n) while maintaining the rightmost Z-box
 *       [*p_l, *p_r] (inclusive). z holds Z[0..i) already computed
 *       (z[0] may be 0 by convention). i==0 → returns 0 without
 *       writing the box. On success updates *p_l and *p_r when the box
 *       extends. Returns Z[i] (match length of s[i..] with s[0..]).
 *       NULL s / p_l / p_r / z, or i>=n → 0.
 *   size_t __gj_z_box_u  (alias)
 *   __libcgj_batch3779_marker = "libcgj-batch3779"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_z_array (batch241) and gj_z_algorithm_prefix (batch1225). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3779_marker[] = "libcgj-batch3779";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One Z-box step at index i. Caller guarantees s/pL/pR/z non-NULL and
 * 0 < i < n. *pL and *pR start as the current box (may be 0,0 with R < i).
 */
static size_t
b3779_z_at(const uint8_t *pS, size_t uN, size_t uI, size_t *pL, size_t *pR,
           const size_t *pZ)
{
	size_t uL;
	size_t uR;
	size_t uZ;
	size_t uK;
	size_t uRem;

	uL = *pL;
	uR = *pR;

	if (uI > uR) {
		/* Outside the box: explicit match from s[0]. */
		uL = uI;
		uR = uI;
		while (uR < uN && pS[uR - uL] == pS[uR]) {
			uR++;
		}
		uZ = uR - uL;
		if (uR > 0u) {
			uR--;
		}
		*pL = uL;
		*pR = uR;
		return uZ;
	}

	/* Inside [L,R]: copy or extend. */
	uK = uI - uL;
	uRem = uR - uI + 1u;
	if (pZ[uK] < uRem) {
		return pZ[uK];
	}

	/* May extend past R. */
	uL = uI;
	while (uR < uN && pS[uR - uL] == pS[uR]) {
		uR++;
	}
	uZ = uR - uL;
	if (uR > 0u) {
		uR--;
	}
	*pL = uL;
	*pR = uR;
	return uZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_z_box_u — Z[i] with Z-box [*p_l, *p_r] maintenance.
 *
 * s:   string bytes of length n
 * n:   string length
 * i:   index to compute (0 → 0 by convention)
 * p_l: in/out left edge of rightmost Z-box
 * p_r: in/out right edge of rightmost Z-box
 * z:   Z values for indices < i
 *
 * Returns Z[i]. Bad args / i>=n → 0. No parent wires.
 */
size_t
gj_z_box_u(const uint8_t *s, size_t n, size_t i, size_t *p_l, size_t *p_r,
           const size_t *z)
{
	(void)NULL;
	if (s == NULL || p_l == NULL || p_r == NULL || z == NULL) {
		return 0u;
	}
	if (i >= n) {
		return 0u;
	}
	if (i == 0u) {
		return 0u;
	}
	return b3779_z_at(s, n, i, p_l, p_r, z);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_z_box_u(const uint8_t *s, size_t n, size_t i, size_t *p_l,
                    size_t *p_r, const size_t *z)
    __attribute__((alias("gj_z_box_u")));
