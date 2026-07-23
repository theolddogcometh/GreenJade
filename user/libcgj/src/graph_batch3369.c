/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3369: CSV skip one field (RFC 4180 lite, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_csv_skip_field_u(const char *s, size_t n, size_t i);
 *     - From index i in s[0..n), skip one CSV field. Quoted fields use
 *       '"' with "" escape. On a trailing comma after the field, the
 *       returned index is just past that comma; otherwise past optional
 *       CR/LF at record end (or at n). On error (NULL s, i > n, unclosed
 *       quote, junk after closing quote) returns n.
 *   size_t __gj_csv_skip_field_u  (alias)
 *   __libcgj_batch3369_marker = "libcgj-batch3369"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_csv_next_field (batch138 out-param walk) and
 * gj_csv_field_count_u (batch3368) — pure index skip; unique name; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3369_marker[] = "libcgj-batch3369";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Skip one field starting at cI. Returns index after field (+ comma if
 * present, else past optional CR/LF). Error → cN.
 */
static size_t
b3369_skip_field(const char *sz, size_t cN, size_t cI)
{
	if (cI > cN) {
		return cN;
	}
	if (cI >= cN) {
		return cN;
	}

	if ((unsigned char)sz[cI] == (unsigned char)'"') {
		cI++;
		for (;;) {
			if (cI >= cN) {
				return cN; /* unclosed */
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
		if (cI < cN) {
			unsigned char uCh = (unsigned char)sz[cI];

			if (uCh != (unsigned char)',' &&
			    uCh != (unsigned char)'\r' &&
			    uCh != (unsigned char)'\n') {
				return cN; /* junk after quote */
			}
		}
	} else {
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
		return cI + 1u;
	}
	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)'\r') {
		cI++;
	}
	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)'\n') {
		cI++;
	}
	return cI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csv_skip_field_u - return index after one CSV field from s[i].
 *
 * Pure index walk; does not copy field content. Error → n.
 */
size_t
gj_csv_skip_field_u(const char *sz, size_t cN, size_t cI)
{
	(void)NULL;
	if (sz == NULL) {
		return cN;
	}
	return b3369_skip_field(sz, cN, cI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_csv_skip_field_u(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_csv_skip_field_u")));
