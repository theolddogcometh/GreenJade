/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3762: Bellman-Ford single-edge relax primitive (_u).
 *
 * Surface (unique symbols):
 *   int gj_bellman_relax_u(int64_t *dist_v, int64_t dist_u, int64_t w);
 *     - One Bellman-Ford edge relax for u→v (signed weights allowed):
 *         cand = dist_u + w;  if cand < *dist_v then *dist_v = cand
 *       INF sentinel is INT64_MAX/4 so two finite legs always sum without
 *       signed overflow. Values with |x| >= INF (or dist_u >= INF) are
 *       treated as non-finite and skip the update. Missing / infinite
 *       *dist_v starts as INF.
 *     Returns 1 if *dist_v was strictly improved, 0 otherwise (including
 *       NULL dist_v or non-finite inputs).
 *   int __gj_bellman_relax_u  (alias)
 *   __libcgj_batch3762_marker = "libcgj-batch3762"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from
 * gj_bellman_ford (batch255) full SSSP — unique single-edge _u surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3762_marker[] = "libcgj-batch3762";

/*
 * Unreachable / non-finite sentinel: INT64_MAX/4.
 * Finite legs each strictly less than this sum without int64 overflow.
 */
#define B3762_INF ((int64_t)(INT64_MAX / 4))

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if v is non-finite under the INF convention. */
static int
b3762_is_inf(int64_t i64V)
{
	return (i64V >= B3762_INF) || (i64V <= -B3762_INF);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bellman_relax_u — relax one signed edge into *dist_v.
 *
 * dist_v: tentative distance at v (updated in place when improved)
 * dist_u: tentative distance at u (INF / non-finite → no update)
 * w:      signed edge weight u→v (non-finite → no update)
 *
 * Returns 1 if improved, else 0. No parent wires.
 */
int
gj_bellman_relax_u(int64_t *dist_v, int64_t dist_u, int64_t w)
{
	int64_t i64Cand;

	(void)NULL;

	if (dist_v == NULL) {
		return 0;
	}
	if (b3762_is_inf(dist_u) || b3762_is_inf(w)) {
		return 0;
	}

	/* Both finite and |x| < INF ⇒ sum fits in int64_t. */
	i64Cand = dist_u + w;
	if (i64Cand < *dist_v) {
		*dist_v = i64Cand;
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bellman_relax_u(int64_t *dist_v, int64_t dist_u, int64_t w)
    __attribute__((alias("gj_bellman_relax_u")));
