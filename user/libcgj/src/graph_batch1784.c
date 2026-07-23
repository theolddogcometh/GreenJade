/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1784: string-builder append C string.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sb_append_str(gj_sb_t *sb, const char *s);
 *     — Append NUL-terminated s (excluding the NUL). NULL s is treated
 *       as empty (success, length unchanged). All-or-nothing: on
 *       insufficient room, state is unchanged. No trailing NUL written
 *       (use gj_sb_terminate). Returns 0 on success, -1 on error.
 *   int __gj_sb_append_str  (alias)
 *   __libcgj_batch1784_marker = "libcgj-batch1784"
 *
 * String-builder exclusive continuum 1781..1790. Distinct from batch351
 * gj_sb_append (pointer form). Self-contained freestanding strlen.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1784_marker[] = "libcgj-batch1784";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Length of a NUL-terminated C string (excluding the NUL).
 * sz must be non-NULL.
 */
static size_t
b1784_strlen(const char *sz)
{
	size_t cb;

	cb = 0u;
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_append_str — append payload of s onto the builder.
 *
 * Copies only; does not write a trailing NUL into the builder buffer.
 */
int
gj_sb_append_str(gj_sb_t *pSb, const char *sz)
{
	size_t cbAdd;
	size_t cbCur;
	size_t i;

	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}

	if (sz == NULL) {
		return 0;
	}

	cbAdd = b1784_strlen(sz);
	if (cbAdd == 0u) {
		return 0;
	}

	cbCur = pSb->len;
	if (cbCur > pSb->cap || cbAdd > pSb->cap - cbCur) {
		return -1;
	}

	for (i = 0u; i < cbAdd; i++) {
		pSb->buf[cbCur + i] = sz[i];
	}
	pSb->len = cbCur + cbAdd;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sb_append_str(gj_sb_t *pSb, const char *sz)
    __attribute__((alias("gj_sb_append_str")));
