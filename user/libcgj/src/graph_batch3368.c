/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3368: CSV field count (RFC 4180 lite, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_csv_field_count_u(const char *s, size_t n);
 *     - Count comma-separated fields in s[0..n). Quoted fields use '"'
 *       with "" escape. Comma separates fields; CR/LF end the record
 *       (not counted as field content). Empty input → 1 (one empty
 *       field). Unclosed quote or NULL with n!=0 → 0 (error). Self-
 *       contained; does not call sibling CSV helpers.
 *   size_t __gj_csv_field_count_u  (alias)
 *   __libcgj_batch3368_marker = "libcgj-batch3368"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_csv_count_fields (batch138) — unique _u name/size_t return; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3368_marker[] = "libcgj-batch3368";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Advance cI past one CSV field (and trailing comma when present).
 * Sets *pfLast = 1 when the field ends the record. Returns new index,
 * or (size_t)-1 on unclosed quote / bad state.
 */
static size_t
b3368_skip_field(const char *sz, size_t cN, size_t cI, int *pfLast)
{
	if (cI > cN) {
		return (size_t)-1;
	}
	if (cI >= cN) {
		/* Empty field at end (e.g. trailing comma already consumed). */
		*pfLast = 1;
		return cI;
	}

	if ((unsigned char)sz[cI] == (unsigned char)'"') {
		/* Quoted field. */
		cI++;
		for (;;) {
			if (cI >= cN) {
				return (size_t)-1; /* unclosed */
			}
			if ((unsigned char)sz[cI] == (unsigned char)'"') {
				if ((cI + 1u) < cN &&
				    (unsigned char)sz[cI + 1u] ==
				    (unsigned char)'"') {
					cI += 2u;
					continue;
				}
				cI++; /* closing quote */
				break;
			}
			cI++;
		}
		/* After close: only comma, CR, LF, or end. */
		if (cI < cN) {
			unsigned char uCh = (unsigned char)sz[cI];

			if (uCh != (unsigned char)',' &&
			    uCh != (unsigned char)'\r' &&
			    uCh != (unsigned char)'\n') {
				return (size_t)-1;
			}
		}
	} else {
		/* Unquoted: until comma / CR / LF / end. */
		while (cI < cN) {
			unsigned char uCh = (unsigned char)sz[cI];

			if (uCh == (unsigned char)',' ||
			    uCh == (unsigned char)'\r' ||
			    uCh == (unsigned char)'\n') {
				break;
			}
			cI++;
		}
	}

	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)',') {
		cI++;
		*pfLast = 0;
		return cI;
	}
	/* Optional CR/LF after last field. */
	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)'\r') {
		cI++;
	}
	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)'\n') {
		cI++;
	}
	*pfLast = 1;
	return cI;
}

/* Count fields; 0 on error. Empty → 1. */
static size_t
b3368_count(const char *sz, size_t cN)
{
	size_t cI;
	size_t cFields;
	int fLast;

	if (cN == 0u) {
		return 1u;
	}

	cI = 0u;
	cFields = 0u;
	for (;;) {
		size_t cNext;

		cNext = b3368_skip_field(sz, cN, cI, &fLast);
		if (cNext == (size_t)-1) {
			return 0u;
		}
		if (cFields == (size_t)-1) {
			return 0u; /* overflow guard */
		}
		cFields++;
		cI = cNext;
		if (fLast != 0) {
			break;
		}
	}
	return cFields;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csv_field_count_u - count RFC 4180-lite fields in s[0..n).
 *
 * Empty buffer → 1. Malformed quoted field / bad args → 0.
 */
size_t
gj_csv_field_count_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL) {
		return (cN == 0u) ? 1u : 0u;
	}
	return b3368_count(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_csv_field_count_u(const char *sz, size_t cN)
    __attribute__((alias("gj_csv_field_count_u")));
