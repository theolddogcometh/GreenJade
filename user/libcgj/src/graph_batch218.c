/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch218: multi-pattern first-match search in a
 * NUL-terminated haystack.
 *
 * Surface (unique symbols):
 *   int gj_multi_find(const char *hay, const char *const *pats,
 *                     size_t npats, size_t *which);
 *     — Find the leftmost occurrence of any of pats[0..npats) in hay.
 *       Returns the byte index of that match in hay (>= 0). On success,
 *       if which is non-NULL, *which is set to the matching pattern
 *       index (lowest index on a position tie). Empty patterns match
 *       at every position (index 0 if hay is non-NULL). Returns -1 if
 *       hay or pats is NULL, npats is 0, or no pattern matches.
 *   __gj_multi_find  (alias)
 *   __libcgj_batch218_marker = "libcgj-batch218"
 *
 * Clean-room freestanding pure C — integer/pointer only (no libc string
 * dependency; -fno-builtin safe). Builds under userspace -msse2; this TU
 * does not require SSE. No third-party source was copied.
 */

#include <stddef.h>

const char __libcgj_batch218_marker[] = "libcgj-batch218";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b218_strlen(const char *s)
{
	size_t n = 0u;

	if (s == NULL) {
		return 0u;
	}
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Return 1 if pat (length nPat) matches hay[pos..] byte-for-byte.
 * Caller guarantees pos + nPat <= nHay.
 */
static int
b218_match_at(const char *hay, size_t pos, const char *pat, size_t nPat)
{
	size_t i;

	for (i = 0u; i < nPat; i++) {
		if (hay[pos + i] != pat[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_multi_find — first (leftmost) match among multiple patterns.
 *
 * For each pattern i in order, scan hay for the earliest occurrence of
 * pats[i]. Track the overall leftmost index and the corresponding
 * pattern index (prefer smaller i when equal). Single linear scan over
 * positions with nested pattern checks; O(|hay| * sum(|pats|)) worst case.
 *
 * Returns match index in [0, strlen(hay)], or -1 on failure / no match.
 */
int
gj_multi_find(const char *hay, const char *const *pats, size_t npats,
    size_t *which)
{
	size_t nHay;
	size_t i;
	size_t pos;
	size_t best_pos;
	size_t best_which;
	int found;

	if (hay == NULL || pats == NULL || npats == 0u) {
		return -1;
	}

	nHay = b218_strlen(hay);
	found = 0;
	best_pos = 0u;
	best_which = 0u;

	for (i = 0u; i < npats; i++) {
		const char *pat;
		size_t nPat;
		size_t last;

		pat = pats[i];
		if (pat == NULL) {
			continue;
		}
		nPat = b218_strlen(pat);

		/* Empty pattern matches at position 0 (and every pos). */
		if (nPat == 0u) {
			if (!found || 0u < best_pos ||
			    (0u == best_pos && i < best_which)) {
				found = 1;
				best_pos = 0u;
				best_which = i;
			}
			continue;
		}

		if (nPat > nHay) {
			continue;
		}

		last = nHay - nPat;
		for (pos = 0u; pos <= last; pos++) {
			if (b218_match_at(hay, pos, pat, nPat)) {
				if (!found || pos < best_pos ||
				    (pos == best_pos && i < best_which)) {
					found = 1;
					best_pos = pos;
					best_which = i;
				}
				/* Leftmost for this pattern; done with i. */
				break;
			}
		}
	}

	if (!found) {
		return -1;
	}

	if (which != NULL) {
		*which = best_which;
	}
	return (int)best_pos;
}

int __gj_multi_find(const char *hay, const char *const *pats, size_t npats,
    size_t *which) __attribute__((alias("gj_multi_find")));
