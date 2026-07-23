/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1229: Aho–Corasick ASCII goto (26-letter) step.
 *
 * Surface (unique symbols):
 *   unsigned gj_aho_goto_ascii(const unsigned *next, unsigned state,
 *                              int ch);
 *     — Follow the goto edge for lowercase letter ch from state in a
 *       dense next[] table laid out as next[state * 26 + (ch - 'a')].
 *       Returns the child state index, or 0 if no edge / invalid ch /
 *       NULL next (0 is the conventional root / missing edge for the
 *       batch219-style layout where root is node 0).
 *   unsigned __gj_aho_goto_ascii  (alias)
 *   __libcgj_batch1229_marker = "libcgj-batch1229"
 *
 * Distinct from trie insert/contains — single goto step only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1229_ALPHA = 26 };

const char __libcgj_batch1229_marker[] = "libcgj-batch1229";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aho_goto_ascii — one Aho–Corasick / trie goto on a-z.
 */
unsigned
gj_aho_goto_ascii(const unsigned *pNext, unsigned uState, int nCh)
{
	int nIdx;

	if (pNext == NULL) {
		return 0u;
	}
	if (nCh < 'a' || nCh > 'z') {
		return 0u;
	}
	nIdx = nCh - 'a';
	return pNext[(size_t)uState * (size_t)B1229_ALPHA + (size_t)nIdx];
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_aho_goto_ascii(const unsigned *pNext, unsigned uState, int nCh)
    __attribute__((alias("gj_aho_goto_ascii")));
