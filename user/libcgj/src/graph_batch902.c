/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch902: append n chars of src onto dst.
 *
 * Surface (unique symbols):
 *   int gj_str_cat_n(char *dst, size_t cap, const char *src, size_t n);
 *     — Append up to n characters from src onto the NUL-terminated
 *       string in dst without writing past dst[cap). Always leaves a
 *       terminator on success. Returns 0 if the entire requested
 *       append completed (or n was 0 / src empty); -1 on bad args,
 *       missing terminator in dst, or insufficient room (all-or-
 *       nothing: dst unchanged when a valid terminator existed).
 *   int __gj_str_cat_n  (alias)
 *   __libcgj_batch902_marker = "libcgj-batch902"
 *
 * Distinct from gj_str_cat_cap (batch352, full-src append) — avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch902_marker[] = "libcgj-batch902";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b902_strnlen(const char *sz, size_t cbMax)
{
	size_t i;

	if (sz == NULL) {
		return 0u;
	}
	for (i = 0u; i < cbMax; i++) {
		if (sz[i] == '\0') {
			return i;
		}
	}
	return cbMax;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cat_n — append up to n characters of src onto dst.
 *
 * dst: buffer holding a NUL-terminated string; capacity is cap bytes
 * cap: total size of dst (must be > 0)
 * src: source bytes; NULL treated as empty
 * n:   maximum characters to take from src (stops early at NUL)
 *
 * Returns 0 on full successful append, -1 on failure.
 */
int
gj_str_cat_n(char *dst, size_t cap, const char *src, size_t n)
{
	size_t cbDst;
	size_t cbSrc;
	size_t i;

	if (dst == NULL || cap == 0u) {
		return -1;
	}

	cbDst = b902_strnlen(dst, cap);
	if (cbDst == cap) {
		/* No NUL found — cannot safely append. */
		return -1;
	}

	if (src == NULL || n == 0u) {
		cbSrc = 0u;
	} else {
		/* At most n payload chars; strnlen bound by n. */
		cbSrc = b902_strnlen(src, n);
	}

	if (cbSrc > (cap - cbDst - 1u)) {
		return -1;
	}

	for (i = 0u; i < cbSrc; i++) {
		dst[cbDst + i] = src[i];
	}
	dst[cbDst + cbSrc] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_cat_n(char *dst, size_t cap, const char *src, size_t n)
    __attribute__((alias("gj_str_cat_n")));
