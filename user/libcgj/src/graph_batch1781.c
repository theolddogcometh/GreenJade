/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1781: string-builder init (struct surface).
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef only (no shared header). Layout matches 1782..1789.
 *   void gj_sbuf_init(gj_sb_t *sb, char *buf, size_t cap);
 *     — Bind sb to caller-owned buffer [buf, buf+cap). Sets len = 0.
 *       If cap > 0 and buf non-NULL, writes buf[0] = '\0' so the builder
 *       starts as an empty C string. NULL sb is a no-op. NULL buf with
 *       non-zero cap zeros the handle (buf=NULL, len=0, cap=0).
 *   void __gj_sbuf_init  (alias)
 *   __libcgj_batch1781_marker = "libcgj-batch1781"
 *
 * String-builder exclusive continuum 1781..1790 (struct sb). Distinct
 * from batch351 gj_sbuf_init(size_t *) which only clears a length pointer
 * (different signature; same public name would multi-def if both were
 * linked — this wave is the struct API exclusive set).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1781_marker[] = "libcgj-batch1781";

/* ---- local state (plain struct typedef) -------------------------------- */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sbuf_init — bind string builder to caller buffer; len = 0.
 *
 * sb:  builder handle (may be NULL → no-op)
 * buf: caller-owned destination storage
 * cap: total capacity of buf in bytes
 */
void
gj_sbuf_init(gj_sb_t *pSb, char *pBuf, size_t cbCap)
{
	if (pSb == NULL) {
		return;
	}
	if (pBuf == NULL && cbCap != 0u) {
		pSb->buf = NULL;
		pSb->len = 0u;
		pSb->cap = 0u;
		return;
	}
	pSb->buf = pBuf;
	pSb->len = 0u;
	pSb->cap = cbCap;
	if (pBuf != NULL && cbCap > 0u) {
		pBuf[0] = '\0';
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sbuf_init(gj_sb_t *pSb, char *pBuf, size_t cbCap)
    __attribute__((alias("gj_sbuf_init")));
