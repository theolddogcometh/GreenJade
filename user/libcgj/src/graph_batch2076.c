/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2076: ASCII C-like identifier predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_ident_ascii(const char *s);
 *     — Return 1 if s matches [A-Za-z_][A-Za-z0-9_]* (non-empty), else 0.
 *       NULL / empty → 0. ASCII only; no locale.
 *   int __gj_str_is_ident_ascii  (alias)
 *   __libcgj_batch2076_marker = "libcgj-batch2076"
 *
 * Str encode exclusive wave (2071–2080). Distinct from gj_str_is_ident
 * (batch827) by the _ascii suffix — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2076_marker[] = "libcgj-batch2076";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2076_is_start(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b2076_is_cont(unsigned char u8C)
{
	if (b2076_is_start(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

static int
b2076_is_ident(const char *sz)
{
	size_t i;

	if (!b2076_is_start((unsigned char)sz[0])) {
		return 0;
	}
	i = 1u;
	while (sz[i] != '\0') {
		if (!b2076_is_cont((unsigned char)sz[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ident_ascii — 1 if s is a non-empty ASCII C-like identifier.
 *
 * First char: A-Z / a-z / '_'. Subsequent: those plus '0'-'9'.
 * s == NULL or empty → 0.
 */
int
gj_str_is_ident_ascii(const char *s)
{
	(void)NULL;
	if (s == NULL || s[0] == '\0') {
		return 0;
	}
	return b2076_is_ident(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ident_ascii(const char *s)
    __attribute__((alias("gj_str_is_ident_ascii")));
