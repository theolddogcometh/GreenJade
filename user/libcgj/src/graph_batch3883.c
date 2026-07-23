/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3883: skip one JSON number (NUL-terminated, _u).
 *
 * Surface (unique symbols):
 *   const char *gj_json_skip_number_u(const char *s);
 *     - From s (NUL-terminated), skip an RFC 8259 number:
 *         -? INT FRAC? EXP?
 *       INT = digits ; FRAC = '.' digits ;
 *       EXP = [eE] [+-]? digits.
 *       Returns pointer after the number on success. On error (NULL s,
 *       empty, not a number start, incomplete fraction/exponent)
 *       returns NULL.
 *   const char *__gj_json_skip_number_u  (alias)
 *   __libcgj_batch3883_marker = "libcgj-batch3883"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_json_skip_number_n (batch2517 bounded index form) — NUL-terminated
 * pointer form; unique name; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3883_marker[] = "libcgj-batch3883";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3883_is_digit(unsigned char u8C)
{
	return (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9')
	    ? 1 : 0;
}

/*
 * Skip number at sz. Returns end pointer or NULL on error.
 * Accepts leading zeros loosely for the integer part after optional '-':
 * requires at least one digit. Does not enforce "no leading zero" strict
 * RFC (scanner-friendly; value parsers may be stricter).
 */
static const char *
b3883_skip_number(const char *sz)
{
	if (sz == NULL || *sz == '\0') {
		return NULL;
	}

	/* optional minus */
	if ((unsigned char)*sz == (unsigned char)'-') {
		sz++;
		if (*sz == '\0') {
			return NULL;
		}
	}

	/* require at least one digit */
	if (b3883_is_digit((unsigned char)*sz) == 0) {
		return NULL;
	}
	while (b3883_is_digit((unsigned char)*sz) != 0) {
		sz++;
	}

	/* optional fraction */
	if ((unsigned char)*sz == (unsigned char)'.') {
		sz++;
		if (b3883_is_digit((unsigned char)*sz) == 0) {
			return NULL;
		}
		while (b3883_is_digit((unsigned char)*sz) != 0) {
			sz++;
		}
	}

	/* optional exponent */
	if ((unsigned char)*sz == (unsigned char)'e' ||
	    (unsigned char)*sz == (unsigned char)'E') {
		sz++;
		if (*sz == '\0') {
			return NULL;
		}
		if ((unsigned char)*sz == (unsigned char)'+' ||
		    (unsigned char)*sz == (unsigned char)'-') {
			sz++;
			if (*sz == '\0') {
				return NULL;
			}
		}
		if (b3883_is_digit((unsigned char)*sz) == 0) {
			return NULL;
		}
		while (b3883_is_digit((unsigned char)*sz) != 0) {
			sz++;
		}
	}

	return sz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_number_u — return pointer after a JSON number at s.
 *
 * s: NUL-terminated buffer; start of number token
 *
 * Pure walk; does not parse numeric value. Error → NULL.
 * No parent wires.
 */
const char *
gj_json_skip_number_u(const char *sz)
{
	(void)NULL;
	return b3883_skip_number(sz);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_json_skip_number_u(const char *sz)
    __attribute__((alias("gj_json_skip_number_u")));
