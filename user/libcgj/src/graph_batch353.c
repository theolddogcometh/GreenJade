/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch353: non-destructive split of a NUL-terminated
 * string on a single delimiter character into byte offsets and lengths.
 *
 * Surface (unique symbols):
 *   size_t gj_split_chr(const char *s, char delim, size_t *offs,
 *                       size_t *lens, size_t max_parts);
 *     — Scan s for delim; for each field write offs[i] (start index into s)
 *       and lens[i] (byte length, not including delim or NUL). Does not
 *       mutate s. Empty fields between consecutive delims (and leading /
 *       trailing delim) are recorded as lens == 0. Empty s yields one
 *       empty field (offs[0] == 0, lens[0] == 0) when max_parts >= 1.
 *       Writes at most max_parts entries; remaining fields after the
 *       capacity is filled are discarded. Returns the number of parts
 *       written (0 .. max_parts). NULL s, NULL offs/lens, or max_parts
 *       == 0 → 0.
 *   size_t __gj_split_chr  (alias)
 *   __libcgj_batch353_marker = "libcgj-batch353"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch353_marker[] = "libcgj-batch353";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_split_chr — split s on delim into offset/length pairs (non-destructive).
 *
 * s:         NUL-terminated input (unchanged)
 * delim:     field separator (compared as unsigned char)
 * offs:      caller array of at least max_parts size_t slots (starts)
 * lens:      caller array of at least max_parts size_t slots (lengths)
 * max_parts: capacity of offs/lens; hard cap on parts written
 *
 * Returns the count of parts stored. Part i is the half-open span
 *   s[offs[i] .. offs[i] + lens[i])
 * which never includes the delimiter or the terminating NUL.
 */
size_t
gj_split_chr(const char *s, char delim, size_t *offs, size_t *lens,
    size_t max_parts)
{
	size_t cParts;
	size_t iStart;
	size_t i;
	unsigned char chDelim;
	unsigned char ch;

	if (s == NULL || offs == NULL || lens == NULL || max_parts == 0u) {
		return 0u;
	}

	chDelim = (unsigned char)delim;
	cParts = 0u;
	iStart = 0u;
	i = 0u;

	for (;;) {
		ch = (unsigned char)s[i];
		if (ch == (unsigned char)'\0' || ch == chDelim) {
			if (cParts < max_parts) {
				offs[cParts] = iStart;
				lens[cParts] = i - iStart;
				cParts++;
			}
			if (ch == (unsigned char)'\0') {
				break;
			}
			/* Advance past delimiter; next field starts here. */
			i++;
			iStart = i;
			if (cParts >= max_parts) {
				/* Capacity full — drop remaining fields. */
				break;
			}
			continue;
		}
		i++;
	}

	return cParts;
}

size_t __gj_split_chr(const char *s, char delim, size_t *offs, size_t *lens,
    size_t max_parts)
    __attribute__((alias("gj_split_chr")));
