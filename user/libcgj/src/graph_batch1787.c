/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1787: string-builder append raw memory.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sb_append_mem(gj_sb_t *sb, const void *src, size_t n);
 *     — Append n bytes from src. NULL src with n == 0 succeeds (no-op).
 *       NULL src with n > 0 fails. All-or-nothing. No trailing NUL
 *       written. Returns 0 on success, -1 on error.
 *   int __gj_sb_append_mem  (alias)
 *   __libcgj_batch1787_marker = "libcgj-batch1787"
 *
 * String-builder exclusive continuum 1781..1790. Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1787_marker[] = "libcgj-batch1787";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Copy cb bytes from pSrc to pDst (no overlap assumed for builder use).
 */
static void
b1787_memcpy(char *pDst, const char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_append_mem — append n raw bytes from src onto the builder.
 */
int
gj_sb_append_mem(gj_sb_t *pSb, const void *pSrc, size_t cbN)
{
	size_t cbCur;
	const char *pBytes;

	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}
	if (cbN == 0u) {
		return 0;
	}
	if (pSrc == NULL) {
		return -1;
	}

	cbCur = pSb->len;
	if (cbCur > pSb->cap || cbN > pSb->cap - cbCur) {
		return -1;
	}

	pBytes = (const char *)pSrc;
	b1787_memcpy(pSb->buf + cbCur, pBytes, cbN);
	pSb->len = cbCur + cbN;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sb_append_mem(gj_sb_t *pSb, const void *pSrc, size_t cbN)
    __attribute__((alias("gj_sb_append_mem")));
