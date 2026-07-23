/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1328: in-place split into at most max token pointer slots.
 *
 * Surface (unique symbols):
 *   size_t gj_str_split_n(char *s, char delim, char **out_ptrs, size_t max);
 *     — Mutate s by writing NUL over each delimiter. Fill out_ptrs[0..k)
 *       with pointers to successive tokens (including empty fields).
 *       Writes at most max entries; returns the number of tokens stored
 *       (0 .. max). NULL s / NULL out_ptrs / max == 0 → 0.
 *   size_t __gj_str_split_n  (alias)
 *   __libcgj_batch1328_marker = "libcgj-batch1328"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1328_marker[] = "libcgj-batch1328";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_split_n — destructive split; fill out_ptrs up to max tokens.
 */
size_t
gj_str_split_n(char *sz, char chDelim, char **ppOut, size_t cMax)
{
	size_t cParts;
	size_t i;
	unsigned char uDelim;
	unsigned char uCh;

	if (sz == NULL || ppOut == NULL || cMax == 0u) {
		return 0u;
	}

	uDelim = (unsigned char)chDelim;
	cParts = 0u;
	i = 0u;

	/* First token always starts at s (even if empty). */
	ppOut[cParts++] = sz;

	for (;;) {
		uCh = (unsigned char)sz[i];
		if (uCh == 0u) {
			break;
		}
		if (uCh == uDelim) {
			sz[i] = '\0';
			i++;
			if (cParts >= cMax) {
				/* Capacity full — stop; rest of string is cut. */
				break;
			}
			ppOut[cParts++] = sz + i;
			continue;
		}
		i++;
	}

	return cParts;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_split_n(char *sz, char chDelim, char **ppOut, size_t cMax)
    __attribute__((alias("gj_str_split_n")));
