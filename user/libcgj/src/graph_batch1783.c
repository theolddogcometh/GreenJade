/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1783: string-builder append one character.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sbuf_append_char(gj_sb_t *sb, char c);
 *     — Append one character c when len < cap. Does not write a trailing
 *       NUL (use gj_sb_terminate). Returns 0 on success, -1 on bad args
 *       or insufficient room.
 *   int __gj_sbuf_append_char  (alias)
 *   __libcgj_batch1783_marker = "libcgj-batch1783"
 *
 * String-builder exclusive continuum 1781..1790 (struct surface).
 * Distinct from batch351 gj_sbuf_append_char(buf,cap,len,c) pointer form.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1783_marker[] = "libcgj-batch1783";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sbuf_append_char — append one character onto the struct builder.
 *
 * Requires at least one free payload byte (len < cap). Does not auto-NUL.
 */
int
gj_sbuf_append_char(gj_sb_t *pSb, char chC)
{
	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}
	if (pSb->len >= pSb->cap) {
		return -1;
	}
	pSb->buf[pSb->len] = chC;
	pSb->len++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sbuf_append_char(gj_sb_t *pSb, char chC)
    __attribute__((alias("gj_sbuf_append_char")));
