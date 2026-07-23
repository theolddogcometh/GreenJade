/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch606: length after trailing ASCII whitespace strip.
 *
 * Surface (unique symbols):
 *   size_t gj_str_rtrim_len(const char *s, size_t n);
 *     — Return the length of the prefix of s[0..n) after stripping
 *       trailing space / tab / CR / LF. Does not write memory; does not
 *       require a NUL terminator inside the n-byte window. s == NULL
 *       or n == 0 → 0.
 *   size_t __gj_str_rtrim_len  (alias)
 *   __libcgj_batch606_marker = "libcgj-batch606"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers (no strlen / isspace). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch606_marker[] = "libcgj-batch606";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is a trailing trim target: space, tab, CR, or LF. */
static int
b606_is_trim(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rtrim_len — length of s[0..n) after stripping trailing whitespace.
 *
 * s: buffer of at least n octets, or NULL
 * n: number of octets in the window (not necessarily NUL-terminated)
 *
 * Walks backward from s[n-1] while characters are space/tab/CR/LF.
 * Returns how many leading octets remain (0..n). NULL s or n == 0 → 0.
 * Does not mutate s.
 */
size_t
gj_str_rtrim_len(const char *s, size_t n)
{
	if (s == NULL || n == 0u) {
		return 0u;
	}

	while (n > 0u && b606_is_trim(s[n - 1u])) {
		n--;
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_rtrim_len(const char *s, size_t n)
    __attribute__((alias("gj_str_rtrim_len")));
