/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1693: B-tree minimum keys (non-root).
 *
 * Surface (unique symbols):
 *   size_t gj_btree_min_keys(size_t t);
 *     — Minimum keys in a non-root node for minimum degree t: t - 1.
 *       t == 0 → 0. (Root may hold fewer keys; caller enforces that.)
 *   size_t __gj_btree_min_keys  (alias)
 *   __libcgj_batch1693_marker = "libcgj-batch1693"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1693_marker[] = "libcgj-batch1693";

/* ---- freestanding helpers ---------------------------------------------- */

/* t - 1, with t == 0 → 0. */
static size_t
b1693_min_keys(size_t cT)
{
	if (cT == 0u) {
		return 0u;
	}
	return cT - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btree_min_keys — minimum key count in a non-root B-tree node (t - 1).
 *
 * cT: minimum degree t
 * Returns cT - 1, or 0 if cT is 0.
 */
size_t
gj_btree_min_keys(size_t cT)
{
	return b1693_min_keys(cT);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_btree_min_keys(size_t cT)
    __attribute__((alias("gj_btree_min_keys")));
