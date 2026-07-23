/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch351: string builder over a caller-owned buffer.
 *
 * Surface (unique symbols):
 *   void gj_sb_init(size_t *len);
 *     — Clear the live length (*len = 0). Caller owns the char buffer
 *       and capacity; this only resets the watermark. No-op if len is NULL.
 *   int  gj_sb_append(char *buf, size_t cap, size_t *len, const char *s);
 *     — Append the NUL-terminated string s to buf starting at *len, keep
 *       the result NUL-terminated, and advance *len by the number of
 *       payload bytes written (excluding the trailing NUL). Returns 0 on
 *       success, -1 on bad args or insufficient room for s and a NUL.
 *       NULL s is treated as empty (success, length unchanged, NUL
 *       rewritten at *len when the buffer can hold it).
 *   int  gj_sb_append_char(char *buf, size_t cap, size_t *len, char c);
 *     — Append one character c, keep NUL-terminated, advance *len by 1.
 *       Returns 0 on success, -1 on bad args or insufficient room for
 *       the character plus trailing NUL.
 *   __gj_sb_init / __gj_sb_append / __gj_sb_append_char  (aliases)
 *   __libcgj_batch351_marker = "libcgj-batch351"
 *
 * Layout (caller provides):
 *   char buf[cap];
 *   size_t len;   // payload length excluding trailing NUL
 *   After init, append paths write buf[0..len] with buf[len] == '\0'.
 *   Capacity must leave one byte for the terminator: payload + 1 <= cap.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch351_marker[] = "libcgj-batch351";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Length of a NUL-terminated C string (excluding the NUL).
 * s must be non-NULL.
 */
static size_t
b351_strlen(const char *sz)
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
 * gj_sb_init — reset the string-builder length watermark to zero.
 * Does not touch the caller buffer (no buf/cap on this surface).
 */
void
gj_sb_init(size_t *len)
{
	if (len == NULL) {
		return;
	}
	*len = 0u;
}

/*
 * gj_sb_append — append NUL-terminated s onto the builder.
 *
 * buf:  caller-owned destination
 * cap:  total capacity of buf in bytes
 * len:  in/out payload length (excluding trailing NUL); must be valid
 * s:    C string to append; NULL means empty
 *
 * On success *len is updated and buf[*len] is '\0'. On failure *len and
 * prior payload are left unchanged (no partial append of multi-byte s).
 */
int
gj_sb_append(char *buf, size_t cap, size_t *len, const char *s)
{
	size_t cbCur;
	size_t cbAdd;
	size_t i;

	if (buf == NULL || len == NULL) {
		return -1;
	}

	cbCur = *len;

	/* Existing length must leave room for at least the terminator. */
	if (cbCur >= cap) {
		return -1;
	}

	if (s == NULL) {
		buf[cbCur] = '\0';
		return 0;
	}

	cbAdd = b351_strlen(s);

	/*
	 * Need room for cbAdd payload bytes plus one trailing NUL:
	 *   cbCur + cbAdd + 1 <= cap
	 * Written as cbAdd >= (cap - cbCur) to avoid size_t wrap and
	 * to require at least one free byte beyond the payload.
	 */
	if (cbAdd >= cap - cbCur) {
		return -1;
	}

	for (i = 0u; i < cbAdd; i++) {
		buf[cbCur + i] = s[i];
	}
	cbCur += cbAdd;
	buf[cbCur] = '\0';
	*len = cbCur;
	return 0;
}

/*
 * gj_sb_append_char — append a single character onto the builder.
 *
 * Same buffer/len contract as gj_sb_append. Requires room for the
 * character and a trailing NUL (two free bytes when *len is at the
 * current payload end, i.e. *len + 1 < cap).
 */
int
gj_sb_append_char(char *buf, size_t cap, size_t *len, char c)
{
	size_t cbCur;

	if (buf == NULL || len == NULL) {
		return -1;
	}

	cbCur = *len;

	/* Need space for c and the trailing NUL. */
	if (cap < 2u || cbCur >= cap - 1u) {
		return -1;
	}

	buf[cbCur] = c;
	cbCur++;
	buf[cbCur] = '\0';
	*len = cbCur;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_sb_init(size_t *len)
    __attribute__((alias("gj_sb_init")));

int __gj_sb_append(char *buf, size_t cap, size_t *len, const char *s)
    __attribute__((alias("gj_sb_append")));

int __gj_sb_append_char(char *buf, size_t cap, size_t *len, char c)
    __attribute__((alias("gj_sb_append_char")));
