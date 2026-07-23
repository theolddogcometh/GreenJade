/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1697: Fenwick (BIT) parent / query step.
 *
 * Surface (unique symbols — distinct from gj_fenwick_add/sum (batch220)):
 *   size_t gj_fenwick_parent(size_t i);
 *     — Next 1-based Fenwick index on a prefix-sum (query) walk:
 *       i - (i & -i), i.e. clear the least-set bit. i == 0 → 0.
 *   size_t __gj_fenwick_parent  (alias)
 *   __libcgj_batch1697_marker = "libcgj-batch1697"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1697_marker[] = "libcgj-batch1697";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least set bit of i (two's complement). i == 0 → 0. */
static size_t
b1697_lsb(size_t uI)
{
	return uI & (size_t)(0u - uI);
}

/* Query parent: i - lsb(i). */
static size_t
b1697_parent(size_t uI)
{
	if (uI == 0u) {
		return 0u;
	}
	return uI - b1697_lsb(uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fenwick_parent — 1-based Fenwick query step (clear least-set bit).
 *
 * uI: current 1-based tree index (0 is identity / empty)
 * Returns uI with its lowest set bit cleared, or 0 when uI is 0.
 */
size_t
gj_fenwick_parent(size_t uI)
{
	return b1697_parent(uI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fenwick_parent(size_t uI)
    __attribute__((alias("gj_fenwick_parent")));
