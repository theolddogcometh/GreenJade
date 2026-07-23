/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4328: freestanding C-string byte count
 * (wave-unique rename).
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_char_u2(const char *s, int c);
 *     - Count occurrences of byte (unsigned char)c in the NUL-terminated
 *       string s (payload octets before NUL only; the terminator is
 *       never counted). s == NULL → 0.
 *       Renamed from gj_str_count_char_u (collision with batch2958).
 *   size_t __gj_str_count_char_u2  (alias)
 *   __libcgj_batch4328_marker = "libcgj-batch4328"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_count_char (batch604) and gj_str_count_char_u (batch2958)
 * — unique gj_str_count_char_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4328_marker[] = "libcgj-batch4328";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4328_count_char(const char *s, int nC)
{
	size_t n;
	unsigned char ch;
	const unsigned char *p;

	if (s == NULL) {
		return 0u;
	}

	ch = (unsigned char)nC;
	n = 0u;
	p = (const unsigned char *)(const void *)s;
	while (*p != (unsigned char)'\0') {
		if (*p == ch) {
			n++;
		}
		p++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_char_u2 - count occurrences of byte c in s.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Walks s until NUL, tallying matches against (unsigned char)c.
 * Returns 0 when s is NULL. Does not call libc. Wave-unique rename:
 * gj_str_count_char_u is owned by batch2958. Self-contained; no parent
 * wires.
 */
size_t
gj_str_count_char_u2(const char *s, int c)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b4328_count_char(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_char_u2(const char *s, int c)
    __attribute__((alias("gj_str_count_char_u2")));
