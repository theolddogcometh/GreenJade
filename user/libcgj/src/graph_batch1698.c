/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1698: Fenwick (BIT) next / update step.
 *
 * Surface (unique symbols — distinct from gj_fenwick_parent (batch1697)
 * and gj_fenwick_add/sum (batch220)):
 *   size_t gj_fenwick_next(size_t i);
 *     — Next 1-based Fenwick index on a point-update walk:
 *       i + (i & -i). i == 0 → 0. Overflow of the add wraps size_t.
 *   size_t __gj_fenwick_next  (alias)
 *   __libcgj_batch1698_marker = "libcgj-batch1698"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1698_marker[] = "libcgj-batch1698";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least set bit of i (two's complement). i == 0 → 0. */
static size_t
b1698_lsb(size_t uI)
{
	return uI & (size_t)(0u - uI);
}

/* Update next: i + lsb(i). */
static size_t
b1698_next(size_t uI)
{
	if (uI == 0u) {
		return 0u;
	}
	return uI + b1698_lsb(uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fenwick_next — 1-based Fenwick update step (add least-set bit).
 *
 * uI: current 1-based tree index (0 is identity / empty)
 * Returns uI + lsb(uI), or 0 when uI is 0. Modular on size_t overflow.
 */
size_t
gj_fenwick_next(size_t uI)
{
	return b1698_next(uI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fenwick_next(size_t uI)
    __attribute__((alias("gj_fenwick_next")));
