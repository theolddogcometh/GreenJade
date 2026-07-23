/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch827: C-like identifier predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_ident(const char *s);
 *     — Return 1 if s matches [A-Za-z_][A-Za-z0-9_]* (non-empty), else 0.
 *       NULL / empty → 0. ASCII only; no locale.
 *   int __gj_str_is_ident  (alias)
 *   __libcgj_batch827_marker = "libcgj-batch827"
 *
 * Distinct from starts-digit / starts-alpha predicates — full-string
 * identifier check; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch827_marker[] = "libcgj-batch827";

/* ---- helpers ----------------------------------------------------------- */

static int
b827_is_alpha_us(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b827_is_alnum_us(unsigned char u8C)
{
	if (b827_is_alpha_us(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ident — 1 if s is a non-empty C-like identifier.
 *
 * First char: A-Z / a-z / '_'. Subsequent: those plus '0'-'9'.
 */
int
gj_str_is_ident(const char *szS)
{
	size_t i;

	if (szS == NULL || szS[0] == '\0') {
		return 0;
	}
	if (!b827_is_alpha_us((unsigned char)szS[0])) {
		return 0;
	}
	i = 1u;
	while (szS[i] != '\0') {
		if (!b827_is_alnum_us((unsigned char)szS[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ident(const char *szS)
    __attribute__((alias("gj_str_is_ident")));
