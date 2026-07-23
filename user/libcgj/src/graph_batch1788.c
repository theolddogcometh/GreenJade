/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1788: string-builder write trailing NUL.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sb_terminate(gj_sb_t *sb);
 *     — Write buf[len] = '\0' when len < cap. Does not advance len
 *       (payload length stays exclusive of the terminator). Returns 0
 *       on success, -1 if sb/buf is NULL or no room for the NUL.
 *   int __gj_sb_terminate  (alias)
 *   __libcgj_batch1788_marker = "libcgj-batch1788"
 *
 * String-builder exclusive continuum 1781..1790. Pair with append
 * surfaces that do not auto-NUL.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1788_marker[] = "libcgj-batch1788";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_terminate — place a C-string terminator at the current end.
 *
 * Requires one free byte (len < cap). len is not increased.
 */
int
gj_sb_terminate(gj_sb_t *pSb)
{
	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}
	if (pSb->len >= pSb->cap) {
		return -1;
	}
	pSb->buf[pSb->len] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sb_terminate(gj_sb_t *pSb)
    __attribute__((alias("gj_sb_terminate")));
