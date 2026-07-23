/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2686: UTF-8 safe truncate length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_trunc_len_u(const unsigned char *s, size_t n,
 *                              size_t max);
 *     - Return the largest length L with L <= min(n, max) such that
 *       s[0..L) does not end mid multi-byte sequence (no split lead).
 *       Structural only: does not enforce full RFC 3629 validity.
 *       Returns 0 if s is NULL and min(n, max) > 0, or if max == 0.
 *   size_t __gj_utf8_trunc_len_u  (alias)
 *   __libcgj_batch2686_marker = "libcgj-batch2686"
 *
 * UTF-8 exclusive wave (2681-2690). Unique trunc surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2686_marker[] = "libcgj-batch2686";

/* ---- freestanding helpers ---------------------------------------------- */

/* Expected total sequence length from lead, or 0 if not a legal lead. */
static int
b2686_seq_len(unsigned char u8Lead)
{
	if (u8Lead < 0x80u) {
		return 1;
	}
	if (u8Lead < 0xC2u) {
		return 0;
	}
	if (u8Lead < 0xE0u) {
		return 2;
	}
	if (u8Lead < 0xF0u) {
		return 3;
	}
	if (u8Lead <= 0xF4u) {
		return 4;
	}
	return 0;
}

/*
 * Largest L <= min(n, max) that does not cut a multi-byte sequence.
 * Walks complete sequences (or single invalid octets as 1-byte steps).
 * p must be non-NULL when end > 0.
 */
static size_t
b2686_trunc(const unsigned char *p, size_t n, size_t max)
{
	size_t uEnd;
	size_t i;
	size_t uLast;

	uEnd = (max < n) ? max : n;
	if (uEnd == 0u) {
		return 0u;
	}

	i = 0u;
	uLast = 0u;
	while (i < uEnd) {
		int nLen = b2686_seq_len(p[i]);

		if (nLen <= 0) {
			/* Illegal lead / bare continuation: consume 1 octet. */
			if (i + 1u > uEnd) {
				break;
			}
			i += 1u;
			uLast = i;
			continue;
		}
		if (i + (size_t)nLen > uEnd) {
			/* Sequence would extend past the cap — stop. */
			break;
		}
		i += (size_t)nLen;
		uLast = i;
	}
	return uLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_trunc_len_u - max prefix length <= max that ends on a boundary.
 *
 * s: buffer (NULL with positive cap -> 0)
 * n: available buffer length
 * max: maximum bytes allowed in the truncated prefix
 *
 * Returns L in 0..min(n,max) where s[0..L) ends on a code-unit boundary
 * for legal multi-byte sequences. Invalid lone octets are treated as
 * single-byte steps so progress is always possible.
 */
size_t
gj_utf8_trunc_len_u(const unsigned char *s, size_t n, size_t max)
{
	size_t uCap;

	(void)NULL;
	uCap = (max < n) ? max : n;
	if (uCap == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2686_trunc(s, n, max);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_utf8_trunc_len_u(const unsigned char *s, size_t n, size_t max)
    __attribute__((alias("gj_utf8_trunc_len_u")));
