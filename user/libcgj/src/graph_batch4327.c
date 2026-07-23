/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4327: freestanding C-string first-char index.
 *
 * Surface (unique symbols):
 *   size_t gj_str_chr_idx_u(const char *s, int c);
 *     - Return the 0-based index of the first occurrence of byte
 *       (unsigned char)c in s, or ~(size_t)0 (SIZE_MAX) if not found
 *       or if s is NULL. The terminator is searchable: c == 0 yields
 *       the length of s (index of NUL) when s is non-NULL.
 *   size_t __gj_str_chr_idx_u  (alias)
 *   __libcgj_batch4327_marker = "libcgj-batch4327"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_chr (batch415 pointer return) — unique index surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4327_marker[] = "libcgj-batch4327";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4327_chr_idx(const char *sz, int nC)
{
	const unsigned char *p;
	unsigned char ch;
	size_t i;

	if (sz == NULL) {
		return ~(size_t)0;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)(const void *)sz;
	i = 0u;
	for (;;) {
		if (p[i] == ch) {
			return i;
		}
		if (p[i] == (unsigned char)'\0') {
			return ~(size_t)0;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_chr_idx_u - index of first byte c in s, or ~(size_t)0.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * When c is 0 and s is non-NULL, returns the length of s (index of the
 * terminator). Does not call libc. Self-contained; no parent wires.
 */
size_t
gj_str_chr_idx_u(const char *s, int c)
{
	(void)NULL;
	return b4327_chr_idx(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_chr_idx_u(const char *s, int c)
    __attribute__((alias("gj_str_chr_idx_u")));
