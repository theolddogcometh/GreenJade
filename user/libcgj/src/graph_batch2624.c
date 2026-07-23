/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2624: freestanding C-string copy (_u wave).
 *
 * Surface (unique symbols):
 *   char *gj_str_cpy_u(char *dst, const char *src);
 *     - Copy NUL-terminated src into dst (including the terminator).
 *       Returns dst. dst == NULL or src == NULL → return dst (no write).
 *       Caller must ensure dst has room for src including NUL.
 *   char *__gj_str_cpy_u  (alias)
 *   __libcgj_batch2624_marker = "libcgj-batch2624"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_cpy (batch412) and gj_str_copy_n_u (batch2201) — unique _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2624_marker[] = "libcgj-batch2624";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2624_cpy(char *dst, const char *src)
{
	size_t i;

	if (dst == NULL || src == NULL) {
		return dst;
	}

	i = 0u;
	for (;;) {
		dst[i] = src[i];
		if (src[i] == '\0') {
			break;
		}
		i++;
	}
	return dst;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cpy_u - copy src into dst including the terminating NUL.
 *
 * dst: destination buffer (must be large enough when both non-NULL)
 * src: NUL-terminated source string
 *
 * Does not call libc strcpy. Returns dst. NULL dst or src → return dst.
 */
char *
gj_str_cpy_u(char *dst, const char *src)
{
	(void)NULL;
	return b2624_cpy(dst, src);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_cpy_u(char *dst, const char *src)
    __attribute__((alias("gj_str_cpy_u")));
