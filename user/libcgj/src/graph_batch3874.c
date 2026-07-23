/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3874: C-like identifier full-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_ident_u(const char *s);
 *     - Return 1 if s matches [A-Za-z_][A-Za-z0-9_]* (non-empty), else 0.
 *       NULL / empty → 0. ASCII only; no locale.
 *   int __gj_str_is_ident_u  (alias)
 *   __libcgj_batch3874_marker = "libcgj-batch3874"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Distinct from
 * gj_str_is_ident (batch827) and gj_str_is_ident_ascii (batch2076) —
 * unique gj_str_is_ident_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3874_marker[] = "libcgj-batch3874";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3874_is_start(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b3874_is_cont(unsigned char u8C)
{
	if (b3874_is_start(u8C) != 0) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

static int
b3874_is_ident(const char *sz)
{
	size_t i;

	if (!b3874_is_start((unsigned char)sz[0])) {
		return 0;
	}
	i = 1u;
	while (sz[i] != '\0') {
		if (!b3874_is_cont((unsigned char)sz[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ident_u - 1 if s is a non-empty C-like identifier.
 *
 * First char: A-Z / a-z / '_'. Subsequent: those plus '0'-'9'.
 * s == NULL or empty → 0. Does not call libc. No parent wires.
 */
int
gj_str_is_ident_u(const char *s)
{
	(void)NULL;
	if (s == NULL || s[0] == '\0') {
		return 0;
	}
	return b3874_is_ident(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ident_u(const char *s)
    __attribute__((alias("gj_str_is_ident_u")));
