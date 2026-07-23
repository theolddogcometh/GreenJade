/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1692: B-tree maximum keys per node.
 *
 * Surface (unique symbols):
 *   size_t gj_btree_max_keys(size_t t);
 *     — Maximum keys in any node for minimum degree t: 2*t - 1.
 *       t == 0 → 0 (undefined order). Overflow of 2*t → 0.
 *   size_t __gj_btree_max_keys  (alias)
 *   __libcgj_batch1692_marker = "libcgj-batch1692"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1692_marker[] = "libcgj-batch1692";

/* ---- freestanding helpers ---------------------------------------------- */

/* 2*t - 1 with overflow → 0. */
static size_t
b1692_max_keys(size_t cT)
{
	size_t cTwice;

	if (cT == 0u) {
		return 0u;
	}
	/* 2*t must not wrap size_t. */
	if (cT > ((size_t)-1 / 2u)) {
		return 0u;
	}
	cTwice = cT * 2u;
	return cTwice - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btree_max_keys — maximum key count in a B-tree node (2*t - 1).
 *
 * cT: minimum degree t
 * Returns 2*cT - 1, or 0 if cT is 0 or 2*cT would overflow.
 */
size_t
gj_btree_max_keys(size_t cT)
{
	return b1692_max_keys(cT);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_btree_max_keys(size_t cT)
    __attribute__((alias("gj_btree_max_keys")));
