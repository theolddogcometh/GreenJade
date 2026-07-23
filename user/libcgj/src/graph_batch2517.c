/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2517: skip one JSON number (bounded index).
 *
 * Surface (unique symbols):
 *   size_t gj_json_skip_number_n(const char *s, size_t n, size_t i);
 *     - From index i in s[0..n), skip an RFC 8259 number:
 *         -? INT FRAC? EXP?
 *       INT = '0' | [1-9] digits ; FRAC = '.' digits ;
 *       EXP = [eE] [+-]? digits.
 *       Returns index after the number on success. On error (NULL s,
 *       i >= n, not a number start, incomplete fraction/exponent)
 *       returns n.
 *   size_t __gj_json_skip_number_n  (alias)
 *   __libcgj_batch2517_marker = "libcgj-batch2517"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from internal
 * skip helpers in batch107 — public bounded form; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2517_marker[] = "libcgj-batch2517";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2517_is_digit(unsigned char u8C)
{
	return (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9')
	    ? 1 : 0;
}

/*
 * Skip number at cI. Returns end index or cN on error.
 * Accepts leading zeros loosely for the integer part after optional '-':
 * requires at least one digit. Does not enforce "no leading zero" strict
 * RFC (scanner-friendly; value parsers may be stricter).
 */
static size_t
b2517_skip_number(const char *sz, size_t cN, size_t cI)
{
	if (sz == NULL || cI >= cN) {
		return cN;
	}

	/* optional minus */
	if ((unsigned char)sz[cI] == (unsigned char)'-') {
		cI++;
		if (cI >= cN) {
			return cN;
		}
	}

	/* require at least one digit */
	if (b2517_is_digit((unsigned char)sz[cI]) == 0) {
		return cN;
	}
	while (cI < cN && b2517_is_digit((unsigned char)sz[cI]) != 0) {
		cI++;
	}

	/* optional fraction */
	if (cI < cN && (unsigned char)sz[cI] == (unsigned char)'.') {
		cI++;
		if (cI >= cN ||
		    b2517_is_digit((unsigned char)sz[cI]) == 0) {
			return cN;
		}
		while (cI < cN &&
		    b2517_is_digit((unsigned char)sz[cI]) != 0) {
			cI++;
		}
	}

	/* optional exponent */
	if (cI < cN &&
	    ((unsigned char)sz[cI] == (unsigned char)'e' ||
	     (unsigned char)sz[cI] == (unsigned char)'E')) {
		cI++;
		if (cI >= cN) {
			return cN;
		}
		if ((unsigned char)sz[cI] == (unsigned char)'+' ||
		    (unsigned char)sz[cI] == (unsigned char)'-') {
			cI++;
			if (cI >= cN) {
				return cN;
			}
		}
		if (b2517_is_digit((unsigned char)sz[cI]) == 0) {
			return cN;
		}
		while (cI < cN &&
		    b2517_is_digit((unsigned char)sz[cI]) != 0) {
			cI++;
		}
	}

	return cI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_number_n - return index after a JSON number at s[i].
 *
 * s: buffer of n bytes
 * n: length of s
 * i: start index of number token
 *
 * Pure index walk; does not parse numeric value. Error → n.
 */
size_t
gj_json_skip_number_n(const char *sz, size_t cN, size_t cI)
{
	(void)NULL;
	return b2517_skip_number(sz, cN, cI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_skip_number_n(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_skip_number_n")));
