/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1789: string-builder clear (reset length).
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   void gj_sb_clear(gj_sb_t *sb);
 *     — Set len = 0. If buf non-NULL and cap > 0, write buf[0] = '\0'
 *       so the builder is an empty C string again. NULL sb is a no-op.
 *       Does not free or rebind the buffer.
 *   void __gj_sb_clear  (alias)
 *   __libcgj_batch1789_marker = "libcgj-batch1789"
 *
 * String-builder exclusive continuum 1781..1790. Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1789_marker[] = "libcgj-batch1789";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_clear — reset payload length to zero; keep buf/cap.
 */
void
gj_sb_clear(gj_sb_t *pSb)
{
	if (pSb == NULL) {
		return;
	}
	pSb->len = 0u;
	if (pSb->buf != NULL && pSb->cap > 0u) {
		pSb->buf[0] = '\0';
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sb_clear(gj_sb_t *pSb)
    __attribute__((alias("gj_sb_clear")));
