/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14723: ASCII digit-span length (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14723(void);
 *     - Returns the compile-time graph batch number for this TU (14723).
 *   size_t gj_ascii_isdigit_span_14723(const char *s, size_t n);
 *     - Returns the count of leading ASCII digits ('0'..'9') in the
 *       first n bytes of s. Returns 0 if s is NULL or n is 0.
 *   uint32_t __gj_batch_id_14723              (alias)
 *   size_t   __gj_ascii_isdigit_span_14723    (alias)
 *   __libcgj_batch14723_marker = "libcgj-batch14723"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior digit-span helpers
 * without this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14723_marker[] = "libcgj-batch14723";

#define B14723_ID  14723u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14723_id(void)
{
	return B14723_ID;
}

static int
b14723_is_digit(unsigned char c)
{
	return (c >= (unsigned char)'0' && c <= (unsigned char)'9') ? 1 : 0;
}

static size_t
b14723_digit_span(const char *s, size_t n)
{
	size_t i;

	if (s == NULL || n == 0u)
		return 0u;
	for (i = 0; i < n; i++) {
		if (b14723_is_digit((unsigned char)s[i]) == 0)
			return i;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14723 - report this TU's graph batch number.
 *
 * Always returns 14723.
 */
uint32_t
gj_batch_id_14723(void)
{
	(void)NULL;
	return b14723_id();
}

/*
 * gj_ascii_isdigit_span_14723 - count leading ASCII digits in a span.
 *
 * Pure integer; no libc. Does not require a trailing NUL.
 */
size_t
gj_ascii_isdigit_span_14723(const char *s, size_t n)
{
	return b14723_digit_span(s, n);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14723(void)
    __attribute__((alias("gj_batch_id_14723")));

size_t __gj_ascii_isdigit_span_14723(const char *s, size_t n)
    __attribute__((alias("gj_ascii_isdigit_span_14723")));
