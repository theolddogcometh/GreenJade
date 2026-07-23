/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1782: string-builder remaining capacity.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   size_t gj_sb_remaining(const gj_sb_t *sb);
 *     — Bytes still free in the builder buffer (cap - len). Returns 0
 *       if sb is NULL, buf is NULL, or len >= cap.
 *   size_t __gj_sb_remaining  (alias)
 *   __libcgj_batch1782_marker = "libcgj-batch1782"
 *
 * String-builder exclusive continuum 1781..1790. Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1782_marker[] = "libcgj-batch1782";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_remaining — free bytes (cap - len) still available for append.
 *
 * Does not reserve a trailing NUL; callers that need a terminator should
 * leave one byte free and call gj_sb_terminate (batch1788).
 */
size_t
gj_sb_remaining(const gj_sb_t *pSb)
{
	if (pSb == NULL || pSb->buf == NULL) {
		return 0u;
	}
	if (pSb->len >= pSb->cap) {
		return 0u;
	}
	return pSb->cap - pSb->len;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_sb_remaining(const gj_sb_t *pSb)
    __attribute__((alias("gj_sb_remaining")));
