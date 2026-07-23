/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch257: Aho–Corasick lite (lowercase a-z only).
 *
 * Surface (unique symbols):
 *   ssize_t gj_ac_search(const char *hay, const char *const *pats,
 *                        size_t npats, size_t *which);
 *     — Multi-pattern first match via a stack-built AC automaton.
 *       Alphabet is fixed lowercase 'a'..'z'. Returns the byte start
 *       index of the leftmost match in hay (>= 0). On success, if which
 *       is non-NULL, *which is the matching pattern index (lowest index
 *       on a start-position tie). Empty patterns match at index 0.
 *       Returns -1 if hay/pats is NULL, npats is 0, a pattern is non-a-z,
 *       the automaton exceeds the stack cap, or no pattern matches.
 *   __gj_ac_search  (alias)
 *   __libcgj_batch257_marker = "libcgj-batch257"
 *
 * Does not redefine gj_multi_find (batch218) or gj_trie_* (batch219).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string. Stack-limited automaton (b257_* helpers). No third-party
 * Aho–Corasick source was copied.
 */

#include <stddef.h>
#include <sys/types.h>

/* Fixed lowercase alphabet a..z */
#define B257_ALPHA 26u
/* Max trie nodes on stack (root + unique prefixes). next[] is n*26. */
#define B257_MAX_NODES 256u
/* Max patterns accepted in one search (plen[] on stack). */
#define B257_MAX_PATS 64u

const char __libcgj_batch257_marker[] = "libcgj-batch257";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map ch to alphabet index 0..25, or return (unsigned)-1 if not 'a'..'z'.
 */
static unsigned
b257_idx(int ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return (unsigned)(ch - 'a');
	}
	return (unsigned)-1;
}

/* Zero unsigned buffer of n words (no memset). */
static void
b257_zero_u(unsigned *p, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		p[i] = 0u;
	}
}

/* Zero size_t buffer of n elements. */
static void
b257_zero_sz(size_t *p, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		p[i] = 0u;
	}
}

/*
 * NUL-terminated length; returns -1 if longer than nMax (missing NUL or
 * overlong). nMax is exclusive of the terminator.
 */
static int
b257_strnlen_cap(const char *sz, int nMax)
{
	int n;

	if (sz == NULL) {
		return -1;
	}
	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return -1;
}

/*
 * Insert one a-z pattern into the trie. out[u] stores (pattern_index+1)
 * for a word ending at node u (lowest index wins on identical strings).
 * Returns 0 on success, -1 on capacity / non-a-z.
 */
static int
b257_trie_insert(unsigned *next, unsigned *out, size_t *nnodes,
    const char *pat, size_t pid)
{
	size_t cur;
	size_t n;
	const char *p;
	unsigned k;
	unsigned child;

	n = *nnodes;
	if (n == 0u) {
		/* Allocate root (B257_MAX_NODES >= 1 by definition). */
		n = 1u;
		*nnodes = 1u;
	}

	cur = 0u;
	for (p = pat; *p != '\0'; p++) {
		k = b257_idx((int)(unsigned char)*p);
		if (k == (unsigned)-1) {
			return -1;
		}

		child = next[cur * B257_ALPHA + (size_t)k];
		if (child == 0u) {
			if (n >= (size_t)B257_MAX_NODES) {
				return -1;
			}
			child = (unsigned)n;
			n++;
			*nnodes = n;
			next[cur * B257_ALPHA + (size_t)k] = child;
		} else if ((size_t)child >= n) {
			return -1;
		}
		cur = (size_t)child;
	}

	/* Terminal: keep lowest pattern index for identical words. */
	if (out[cur] == 0u || (unsigned)(pid + 1u) < out[cur]) {
		out[cur] = (unsigned)(pid + 1u);
	}
	return 0;
}

/*
 * Build failure links via BFS. fail[0] = 0. Children of root fail to root.
 * Does not fill dictionary transitions; search walks fail on missing edge.
 */
static void
b257_build_fail(unsigned *next, unsigned *fail, size_t nnodes)
{
	unsigned q[B257_MAX_NODES];
	size_t qh;
	size_t qt;
	unsigned k;
	unsigned u;
	unsigned v;
	unsigned f;
	unsigned child;

	b257_zero_u(fail, nnodes);
	fail[0] = 0u;
	qh = 0u;
	qt = 0u;

	/* Root children: fail → root, enqueue. */
	for (k = 0u; k < B257_ALPHA; k++) {
		child = next[(size_t)k];
		if (child != 0u) {
			fail[child] = 0u;
			q[qt++] = child;
		}
	}

	while (qh < qt) {
		u = q[qh++];
		for (k = 0u; k < B257_ALPHA; k++) {
			v = next[(size_t)u * B257_ALPHA + (size_t)k];
			if (v == 0u) {
				continue;
			}
			/* fail[v] = transition of fail[u] on k (or root). */
			f = fail[u];
			for (;;) {
				if (next[(size_t)f * B257_ALPHA + (size_t)k]
				    != 0u) {
					break;
				}
				if (f == 0u) {
					break;
				}
				f = fail[f];
			}
			child = next[(size_t)f * B257_ALPHA + (size_t)k];
			if (child != 0u && child != v) {
				fail[v] = child;
			} else {
				fail[v] = 0u;
			}
			q[qt++] = v;
		}
	}
}

/*
 * Consider all outputs along the fail chain of state; update best match
 * (leftmost start, then lowest pattern index). end_i is inclusive end
 * index of the current character in hay (0-based).
 */
static void
b257_consider(const unsigned *fail, const unsigned *out,
    const size_t *plen, unsigned state, size_t end_i,
    int *found, size_t *best_start, size_t *best_which)
{
	unsigned u;
	unsigned oid;
	size_t pid;
	size_t start;
	size_t L;

	u = state;
	for (;;) {
		oid = out[u];
		if (oid != 0u) {
			pid = (size_t)(oid - 1u);
			L = plen[pid];
			/* Non-empty patterns only reach here via insert. */
			if (L > 0u && end_i + 1u >= L) {
				start = end_i + 1u - L;
				if (!*found || start < *best_start ||
				    (start == *best_start &&
				     pid < *best_which)) {
					*found = 1;
					*best_start = start;
					*best_which = pid;
				}
			}
		}
		if (u == 0u) {
			break;
		}
		u = fail[u];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ac_search — first multi-pattern match via stack Aho–Corasick (a-z).
 *
 * hay:  NUL-terminated haystack (any bytes; non-a-z reset the state).
 * pats: array of npats NUL-terminated patterns (must be pure a-z or empty).
 * which: optional out — pattern index of the chosen match.
 *
 * Returns start index in hay, or -1. Leftmost start wins; on a start tie
 * the lowest pattern index wins (same convention as gj_multi_find).
 * Empty pattern → 0 (and *which set to the lowest empty pattern index).
 * Automaton is built entirely on the stack (capped nodes/patterns).
 */
ssize_t
gj_ac_search(const char *hay, const char *const *pats, size_t npats,
    size_t *which)
{
	/* next[u*26 + k] = child of node u on letter k; 0 = none. */
	unsigned next[B257_MAX_NODES * B257_ALPHA];
	unsigned fail[B257_MAX_NODES];
	unsigned out[B257_MAX_NODES];
	size_t plen[B257_MAX_PATS];
	size_t nnodes;
	size_t i;
	size_t best_start;
	size_t best_which;
	int found;
	int has_pat;
	unsigned state;
	unsigned k;
	const char *p;

	if (hay == NULL || pats == NULL || npats == 0u) {
		return (ssize_t)-1;
	}
	if (npats > (size_t)B257_MAX_PATS) {
		return (ssize_t)-1;
	}

	/* Empty pattern matches at 0 (lowest empty index). */
	for (i = 0u; i < npats; i++) {
		if (pats[i] != NULL && pats[i][0] == '\0') {
			if (which != NULL) {
				*which = i;
			}
			return (ssize_t)0;
		}
	}

	b257_zero_u(next, (size_t)B257_MAX_NODES * (size_t)B257_ALPHA);
	b257_zero_u(out, (size_t)B257_MAX_NODES);
	b257_zero_sz(plen, (size_t)B257_MAX_PATS);
	nnodes = 0u;
	has_pat = 0;

	for (i = 0u; i < npats; i++) {
		int nLen;

		if (pats[i] == NULL) {
			continue;
		}
		/* Cap pattern length by remaining node budget + generous. */
		nLen = b257_strnlen_cap(pats[i], (int)B257_MAX_NODES);
		if (nLen < 0) {
			return (ssize_t)-1;
		}
		if (nLen == 0) {
			/* Already handled empty above; skip. */
			continue;
		}
		/* Validate a-z and record length. */
		for (p = pats[i]; *p != '\0'; p++) {
			if (b257_idx((int)(unsigned char)*p) == (unsigned)-1) {
				return (ssize_t)-1;
			}
		}
		plen[i] = (size_t)nLen;
		if (b257_trie_insert(next, out, &nnodes, pats[i], i) != 0) {
			return (ssize_t)-1;
		}
		has_pat = 1;
	}

	if (!has_pat || nnodes == 0u) {
		return (ssize_t)-1;
	}

	b257_build_fail(next, fail, nnodes);

	/* Scan hay; track leftmost start (full scan for multi_find parity). */
	found = 0;
	best_start = 0u;
	best_which = 0u;
	state = 0u;

	for (i = 0u; hay[i] != '\0'; i++) {
		k = b257_idx((int)(unsigned char)hay[i]);
		if (k == (unsigned)-1) {
			/* Non-alphabet byte: reset automaton. */
			state = 0u;
			continue;
		}

		while (state != 0u &&
		    next[(size_t)state * B257_ALPHA + (size_t)k] == 0u) {
			state = fail[state];
		}
		if (next[(size_t)state * B257_ALPHA + (size_t)k] != 0u) {
			state = next[(size_t)state * B257_ALPHA + (size_t)k];
		} else {
			state = 0u;
		}

		b257_consider(fail, out, plen, state, i, &found, &best_start,
		    &best_which);
	}

	if (!found) {
		return (ssize_t)-1;
	}
	if (which != NULL) {
		*which = best_which;
	}
	return (ssize_t)best_start;
}

ssize_t __gj_ac_search(const char *hay, const char *const *pats, size_t npats,
    size_t *which) __attribute__((alias("gj_ac_search")));
