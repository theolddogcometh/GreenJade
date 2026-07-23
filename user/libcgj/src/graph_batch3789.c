/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3789: Aho–Corasick output/dictionary link (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_aho_output_u(const unsigned *fail,
 *                            const unsigned char *term, unsigned state);
 *     - Walk the failure chain from state and return the nearest state
 *       on that chain (including `state` itself) for which term[s] != 0,
 *       or 0 if none (root / no match). term[s] marks automaton states
 *       that end a dictionary word. NULL fail or NULL term → 0.
 *       Bounded walk guards corrupted fail cycles.
 *   unsigned __gj_aho_output_u  (alias)
 *   __libcgj_batch3789_marker = "libcgj-batch3789"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_aho_fail_u / gj_aho_goto_ascii — dictionary-link probe only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3789_marker[] = "libcgj-batch3789";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aho_output_u — nearest terminal state on the fail chain of state.
 *
 * Does not allocate. No parent wires.
 */
unsigned
gj_aho_output_u(const unsigned *fail, const unsigned char *term,
                unsigned state)
{
	unsigned steps;

	(void)NULL;
	if (fail == NULL || term == NULL) {
		return 0u;
	}

	steps = 0u;
	for (;;) {
		if (term[state] != 0u) {
			return state;
		}
		if (state == 0u) {
			return 0u;
		}
		state = fail[state];
		steps++;
		if (steps > 65536u) {
			return 0u;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_aho_output_u(const unsigned *fail, const unsigned char *term,
                           unsigned state)
    __attribute__((alias("gj_aho_output_u")));
