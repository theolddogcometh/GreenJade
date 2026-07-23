/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2625: freestanding C-string append (_u wave).
 *
 * Surface (unique symbols):
 *   char *gj_str_cat_u(char *dst, const char *src);
 *     - Append NUL-terminated src onto the NUL-terminated string in dst,
 *       then write a terminating NUL. Returns dst. dst == NULL → NULL.
 *       src == NULL leaves dst unchanged (still returns dst). Caller
 *       must ensure room for existing content plus src plus one NUL.
 *   char *__gj_str_cat_u  (alias)
 *   __libcgj_batch2625_marker = "libcgj-batch2625"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_cat_cap (batch352), gj_str_cat_n_u (batch2202), and
 * gj_str_ncat_u (batch2629) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2625_marker[] = "libcgj-batch2625";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of sz up to (but not including) the first NUL. sz must be non-NULL. */
static size_t
b2625_len(const char *sz)
{
	size_t i;

	i = 0u;
	while (sz[i] != '\0') {
		i++;
	}
	return i;
}

static char *
b2625_cat(char *dst, const char *src)
{
	size_t cbDst;
	size_t i;

	if (dst == NULL) {
		return NULL;
	}
	if (src == NULL) {
		return dst;
	}

	cbDst = b2625_len(dst);
	i = 0u;
	for (;;) {
		dst[cbDst + i] = src[i];
		if (src[i] == '\0') {
			break;
		}
		i++;
	}
	return dst;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cat_u - append src onto dst and force a terminating NUL.
 *
 * dst: NUL-terminated destination (must have room for append + NUL)
 * src: source bytes; NULL treated as empty
 *
 * Returns dst (or NULL if dst is NULL). Does not call libc strcat.
 */
char *
gj_str_cat_u(char *dst, const char *src)
{
	(void)NULL;
	return b2625_cat(dst, src);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_cat_u(char *dst, const char *src)
    __attribute__((alias("gj_str_cat_u")));
