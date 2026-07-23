/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3875: C99 keyword full-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_keyword_u(const char *s);
 *     - Return 1 if s exactly matches a C99 keyword (case-sensitive
 *       full string compare), else 0. NULL / empty → 0.
 *       Keywords: auto break case char const continue default do
 *       double else enum extern float for goto if inline int long
 *       register restrict return short signed sizeof static struct
 *       switch typedef union unsigned void volatile while _Bool
 *       _Complex _Imaginary.
 *   int __gj_str_is_keyword_u  (alias)
 *   __libcgj_batch3875_marker = "libcgj-batch3875"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Unique
 * gj_str_is_keyword_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3875_marker[] = "libcgj-batch3875";

/* C99 keyword table (NUL-terminated string literals). */
static const char *const b3875_kw[] = {
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"register",
	"restrict",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	"_Bool",
	"_Complex",
	"_Imaginary",
};

#define B3875_NKW  (sizeof(b3875_kw) / sizeof(b3875_kw[0]))

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if a and b are equal as C strings (no libc strcmp). */
static int
b3875_streq(const char *a, const char *b)
{
	size_t i;

	i = 0u;
	for (;;) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == '\0') {
			return 1;
		}
		i++;
	}
}

static int
b3875_is_keyword(const char *sz)
{
	size_t i;

	for (i = 0u; i < B3875_NKW; i++) {
		if (b3875_streq(sz, b3875_kw[i]) != 0) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_keyword_u - 1 if s is exactly a C99 keyword.
 *
 * Exact, case-sensitive match against the fixed keyword table.
 * s == NULL or empty → 0. Does not call libc. No parent wires.
 */
int
gj_str_is_keyword_u(const char *s)
{
	(void)NULL;
	if (s == NULL || s[0] == '\0') {
		return 0;
	}
	return b3875_is_keyword(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_keyword_u(const char *s)
    __attribute__((alias("gj_str_is_keyword_u")));
