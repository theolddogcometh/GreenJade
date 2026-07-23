/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3788: Aho–Corasick failure-link step (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_aho_fail_u(const unsigned *next, unsigned alpha,
 *                          const unsigned *fail, unsigned parent,
 *                          unsigned edge);
 *     - Compute the failure link for the child of `parent` reached by
 *       edge index `edge` (0..alpha-1) in a dense goto table laid out
 *       as next[state * alpha + c]. Missing edges are 0 (root). Walks
 *       the fail chain of parent until a goto on `edge` exists (or
 *       root). Returns that state. NULL next/fail, alpha == 0, or
 *       edge >= alpha → 0. Root parent with no deeper match → 0.
 *       Caller must have fail[parent] (and its chain) already valid;
 *       this is the classic single-node fail construction step.
 *   unsigned __gj_aho_fail_u  (alias)
 *   __libcgj_batch3788_marker = "libcgj-batch3788"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_aho_goto_ascii (batch1229 single goto) — fail-link construction
 * step; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3788_marker[] = "libcgj-batch3788";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b3788_goto(const unsigned *next, unsigned alpha, unsigned state,
           unsigned edge)
{
	return next[(size_t)state * (size_t)alpha + (size_t)edge];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aho_fail_u — failure link for child(parent, edge).
 *
 * Does not allocate. No parent wires.
 */
unsigned
gj_aho_fail_u(const unsigned *next, unsigned alpha, const unsigned *fail,
              unsigned parent, unsigned edge)
{
	unsigned state;
	unsigned g;
	/* Bound fail-chain walks for pathological tables. */
	unsigned steps;

	(void)NULL;
	if (next == NULL || fail == NULL || alpha == 0u || edge >= alpha) {
		return 0u;
	}

	/* Start from fail[parent], not parent itself. */
	state = fail[parent];
	steps = 0u;
	for (;;) {
		g = b3788_goto(next, alpha, state, edge);
		if (g != 0u) {
			return g;
		}
		if (state == 0u) {
			return 0u;
		}
		state = fail[state];
		steps++;
		/* Guard against cycles in a corrupted fail table. */
		if (steps > 65536u) {
			return 0u;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_aho_fail_u(const unsigned *next, unsigned alpha,
                         const unsigned *fail, unsigned parent,
                         unsigned edge)
    __attribute__((alias("gj_aho_fail_u")));
