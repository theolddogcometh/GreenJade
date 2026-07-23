/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch943: count whitespace-separated words.
 *
 * Surface (unique symbols):
 *   size_t gj_str_word_count(const char *s);
 *     — Count words in s. Word = maximal run of non-space octets.
 *       Space = ' ', '\t', '\n', '\r', '\f', '\v'. NULL/empty → 0.
 *   size_t __gj_str_word_count  (alias)
 *   __libcgj_batch943_marker = "libcgj-batch943"

 *
 * Does NOT redefine gj_str_count_lines / gj_str_line_at.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch943_marker[] = "libcgj-batch943";

/* ---- helpers ----------------------------------------------------------- */

static int
b943_is_space(unsigned char u8C)
{
	return (u8C == ' ' || u8C == '\t' || u8C == '\n' ||
	        u8C == '\r' || u8C == '\f' || u8C == '\v');
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_word_count — count whitespace-separated tokens in s.
 */
size_t
gj_str_word_count(const char *pS)
{
	size_t cWords;
	const char *p;
	int fInWord;

	if (pS == NULL) {
		return 0u;
	}

	cWords = 0u;
	fInWord = 0;
	p = pS;
	while (*p != '\0') {
		if (b943_is_space((unsigned char)*p)) {
			fInWord = 0;
		} else if (!fInWord) {
			fInWord = 1;
			cWords++;
		}
		p++;
	}
	return cWords;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_word_count(const char *pS)
    __attribute__((alias("gj_str_word_count")));
