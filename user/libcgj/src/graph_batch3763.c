/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3763: Floyd-Warshall single-cell update (_u).
 *
 * Surface (unique symbols):
 *   int gj_floyd_cell_u(uint64_t *dij, uint64_t dik, uint64_t dkj);
 *     - One Floyd-Warshall triple step for path i → k → j:
 *         cand = sat_add(dik, dkj);  if cand < *dij then *dij = cand
 *       UINT64_MAX is the unreachable / missing-edge sentinel. If either
 *       leg is UINT64_MAX the candidate is non-finite and no update
 *       occurs. Saturated add never wraps.
 *     Returns 1 if *dij was strictly improved, 0 otherwise (including
 *       NULL dij or non-finite legs).
 *   int __gj_floyd_cell_u  (alias)
 *   __libcgj_batch3763_marker = "libcgj-batch3763"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from
 * gj_floyd_warshall (batch256) full APSP — unique single-cell _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3763_marker[] = "libcgj-batch3763";

/* Unreachable / missing-edge sentinel. */
#define B3763_INF ((uint64_t)UINT64_MAX)

/* ---- freestanding helpers ---------------------------------------------- */

/* min(a + b, INF) without wrap. */
static uint64_t
b3763_add_sat(uint64_t uA, uint64_t uB)
{
	if (uA >= B3763_INF || uB >= B3763_INF) {
		return B3763_INF;
	}
	if (uB > B3763_INF - uA) {
		return B3763_INF;
	}
	return uA + uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_floyd_cell_u — consider path i→k→j against *dij.
 *
 * dij: current best i→j (updated in place when improved)
 * dik: best i→k
 * dkj: best k→j
 *
 * Returns 1 if improved, else 0. No parent wires.
 */
int
gj_floyd_cell_u(uint64_t *dij, uint64_t dik, uint64_t dkj)
{
	uint64_t uCand;

	(void)NULL;

	if (dij == NULL) {
		return 0;
	}
	if (dik >= B3763_INF || dkj >= B3763_INF) {
		return 0;
	}

	uCand = b3763_add_sat(dik, dkj);
	if (uCand < *dij) {
		*dij = uCand;
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_floyd_cell_u(uint64_t *dij, uint64_t dik, uint64_t dkj)
    __attribute__((alias("gj_floyd_cell_u")));
