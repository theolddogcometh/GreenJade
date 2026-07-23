/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2201: bounded n-octet C-string copy (strncpy-shaped).
 *
 * Surface (unique symbols):
 *   char *gj_str_copy_n_u(char *dst, const char *src, size_t n);
 *     - Copy up to n octets from src into dst. If src is shorter than n,
 *       pad the remainder of the n-octet window with NUL. If src is
 *       longer than or equal to n, copy exactly n octets and do not
 *       force a trailing NUL (classic strncpy semantics). dst == NULL
 *       or n == 0 → return dst (no write). src == NULL treated as empty
 *       (fill the n-octet window with NUL when dst is non-NULL and n>0).
 *   char *__gj_str_copy_n_u  (alias)
 *   __libcgj_batch2201_marker = "libcgj-batch2201"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_cpy
 * (batch412), gj_str_dup_n (batch901), and libc strncpy - unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2201_marker[] = "libcgj-batch2201";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2201_copy_n(char *dst, const char *src, size_t n)
{
	size_t i;

	if (dst == NULL || n == 0u) {
		return dst;
	}

	i = 0u;
	if (src != NULL) {
		while (i < n && src[i] != '\0') {
			dst[i] = src[i];
			i++;
		}
	}
	while (i < n) {
		dst[i] = '\0';
		i++;
	}
	return dst;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_copy_n_u - copy at most n octets of src into dst (pad with NUL).
 *
 * dst: destination buffer of at least n octets when n > 0
 * src: NUL-terminated source, or NULL (treated as empty)
 * n:   maximum octets to write into dst
 *
 * Returns dst. Does not call libc strncpy.
 */
char *
gj_str_copy_n_u(char *dst, const char *src, size_t n)
{
	(void)NULL;
	return b2201_copy_n(dst, src, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_copy_n_u(char *dst, const char *src, size_t n)
    __attribute__((alias("gj_str_copy_n_u")));
