/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1234: unquoted SQL identifier predicate.
 *
 * Surface (unique symbols):
 *   int gj_sql_ident_ok(const char *s);
 *     — Return 1 if s is a non-empty unquoted SQL-like identifier:
 *       [A-Za-z_][A-Za-z0-9_]* with length 1..128. NULL / empty /
 *       overlong / other octets → 0. ASCII only; no quoted "ident".
 *   int __gj_sql_ident_ok  (alias)
 *   __libcgj_batch1234_marker = "libcgj-batch1234"
 *
 * Distinct from gj_str_is_ident (batch827) — SQL-named, length-capped.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1234_MAX_LEN = 128 };

const char __libcgj_batch1234_marker[] = "libcgj-batch1234";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1234_is_start(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b1234_is_cont(unsigned char u8C)
{
	if (b1234_is_start(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sql_ident_ok — 1 if s is a valid unquoted SQL-like identifier.
 */
int
gj_sql_ident_ok(const char *szS)
{
	size_t i;

	if (szS == NULL || szS[0] == '\0') {
		return 0;
	}
	if (!b1234_is_start((unsigned char)szS[0])) {
		return 0;
	}

	i = 1u;
	while (szS[i] != '\0') {
		if (i >= (size_t)B1234_MAX_LEN) {
			return 0;
		}
		if (!b1234_is_cont((unsigned char)szS[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sql_ident_ok(const char *szS)
    __attribute__((alias("gj_sql_ident_ok")));
