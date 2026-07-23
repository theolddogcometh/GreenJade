/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch802: freestanding whitespace-octet counter.
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_ws(const char *s);
 *     — Count space/tab/CR/LF octets in s. s == NULL → 0.
 *   size_t __gj_str_count_ws  (alias)
 *   __libcgj_batch802_marker = "libcgj-batch802"
 *
 * Distinct from gj_str_has_ws (batch801 boolean) / gj_str_is_blank
 * (batch761) — unique gj_str_count_ws surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch802_marker[] = "libcgj-batch802";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is ASCII whitespace: space, HT, LF, or CR. */
static int
b802_is_ws(unsigned char uCh)
{
	return (uCh == (unsigned char)' '
	    || uCh == (unsigned char)'\t'
	    || uCh == (unsigned char)'\n'
	    || uCh == (unsigned char)'\r') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_ws — number of space/tab/CR/LF octets in s.
 *
 * s: NUL-terminated C string (NULL → 0)
 *
 * Does not treat vertical tab / form feed as whitespace. ASCII only.
 */
size_t
gj_str_count_ws(const char *sz)
{
	size_t i;
	size_t cN;

	if (sz == NULL) {
		return 0u;
	}
	cN = 0u;
	for (i = 0u; sz[i] != '\0'; i++) {
		if (b802_is_ws((unsigned char)sz[i])) {
			cN++;
		}
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_ws(const char *sz)
    __attribute__((alias("gj_str_count_ws")));
