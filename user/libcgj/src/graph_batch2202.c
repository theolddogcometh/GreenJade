/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2202: bounded n-octet C-string append (strncat-shaped).
 *
 * Surface (unique symbols):
 *   char *gj_str_cat_n_u(char *dst, const char *src, size_t n);
 *     - Append at most n payload octets from src onto the NUL-terminated
 *       string in dst, then always write a terminating NUL. Stops early
 *       at src NUL. n == 0 or src == NULL leaves dst unchanged (still
 *       returns dst). dst == NULL → NULL. Caller must ensure room for
 *       the existing content plus up to n octets plus one NUL.
 *   char *__gj_str_cat_n_u  (alias)
 *   __libcgj_batch2202_marker = "libcgj-batch2202"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_cat_n
 * (batch902 all-or-nothing cap form) and gj_str_cat_cap (batch352) -
 * unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2202_marker[] = "libcgj-batch2202";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of sz up to (but not including) the first NUL. sz must be non-NULL. */
static size_t
b2202_len(const char *sz)
{
	size_t i;

	i = 0u;
	while (sz[i] != '\0') {
		i++;
	}
	return i;
}

static char *
b2202_cat_n(char *dst, const char *src, size_t n)
{
	size_t cbDst;
	size_t i;

	if (dst == NULL) {
		return NULL;
	}
	if (src == NULL || n == 0u) {
		return dst;
	}

	cbDst = b2202_len(dst);
	i = 0u;
	while (i < n && src[i] != '\0') {
		dst[cbDst + i] = src[i];
		i++;
	}
	dst[cbDst + i] = '\0';
	return dst;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cat_n_u - append at most n octets of src onto dst + force NUL.
 *
 * dst: NUL-terminated destination (must have room for append + NUL)
 * src: source bytes; NULL treated as empty
 * n:   maximum payload characters to take from src
 *
 * Returns dst (or NULL if dst is NULL). Does not call libc strncat.
 */
char *
gj_str_cat_n_u(char *dst, const char *src, size_t n)
{
	(void)NULL;
	return b2202_cat_n(dst, src, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_cat_n_u(char *dst, const char *src, size_t n)
    __attribute__((alias("gj_str_cat_n_u")));
