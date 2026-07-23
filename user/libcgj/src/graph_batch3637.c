/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3637: right-pad buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_pad_right_need_u(const char *s, size_t width);
 *     - Bytes needed (including trailing NUL) for the string that
 *       gj_str_pad_right (batch1083) would write for (s, width, ...).
 *       Field width is max(strlen(s), width); result size is field + 1.
 *       NULL s → 0. Overflow of size_t → 0.
 *   size_t __gj_str_pad_right_need_u  (alias)
 *   __libcgj_batch3637_marker = "libcgj-batch3637"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_pad_right (batch1083 write) — length-only surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3637_marker[] = "libcgj-batch3637";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3637_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static size_t
b3637_pad_right_need(const char *sz, size_t cbWidth)
{
	size_t cbSrc;
	size_t cbField;

	if (sz == NULL) {
		return 0u;
	}

	cbSrc = b3637_len(sz);
	cbField = cbSrc;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	if (cbField > SIZE_MAX - 1u) {
		return 0u;
	}
	return cbField + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_pad_right_need_u - bytes needed (incl. NUL) for right-pad of s.
 *
 * s:     subject string (NULL → 0)
 * width: minimum field width in characters
 *
 * Examples (return includes trailing NUL):
 *   ("hi", 5) → 6  ("hi   " + NUL)
 *   ("hi", 1) → 3  ("hi" + NUL)
 *   ("", 3)   → 4
 *   (NULL, 4) → 0
 *
 * Length-only; pad character does not affect size. No parent wires.
 */
size_t
gj_str_pad_right_need_u(const char *s, size_t width)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3637_pad_right_need(s, width);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_pad_right_need_u(const char *s, size_t width)
    __attribute__((alias("gj_str_pad_right_need_u")));
