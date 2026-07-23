/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1691: B-tree minimum degree validity.
 *
 * Surface (unique symbols):
 *   int gj_btree_min_degree_ok(size_t t);
 *     — Return 1 if t is a valid B-tree minimum degree (t >= 2), else 0.
 *       Classic CLRS: every non-root has at least t-1 keys; a node holds
 *       at most 2t-1 keys. t == 0 and t == 1 are rejected.
 *   int __gj_btree_min_degree_ok  (alias)
 *   __libcgj_batch1691_marker = "libcgj-batch1691"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1691_marker[] = "libcgj-batch1691";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when t is a legal B-tree min degree. */
static int
b1691_t_ok(size_t cT)
{
	return (cT >= 2u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btree_min_degree_ok — 1 if t is a valid B-tree minimum degree.
 *
 * cT: proposed minimum degree t
 * Returns 1 when cT >= 2, else 0.
 */
int
gj_btree_min_degree_ok(size_t cT)
{
	return b1691_t_ok(cT);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_btree_min_degree_ok(size_t cT)
    __attribute__((alias("gj_btree_min_degree_ok")));
