/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3589: skip leading ASCII whitespace (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_ws_skip_u(const unsigned char *s, size_t n);
 *     - Return the number of leading ASCII whitespace octets in
 *       s[0..n) (space / HT / LF / VT / FF / CR). Stops at first
 *       non-space or end of window. NULL s or n == 0 → 0. Does not
 *       require NUL termination.
 *   size_t __gj_ws_skip_u  (alias)
 *   __libcgj_batch3589_marker = "libcgj-batch3589"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from string
 * trim helpers and WebSocket gj_ws_* frames — unique ws_skip_u
 * surface; no multi-def. No parent wires. Self-contained (does not
 * call gj_ws_is_space_u).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3589_marker[] = "libcgj-batch3589";

/* ---- freestanding helpers ---------------------------------------------- */

/* C-locale ASCII whitespace: HT LF VT FF CR SP. */
static int
b3589_is_space(unsigned char u8C)
{
	if (u8C == (unsigned char)' ') {
		return 1;
	}
	if (u8C >= 0x09u && u8C <= 0x0Du) {
		return 1;
	}
	return 0;
}

/*
 * Count leading whitespace. pS non-NULL, cN > 0.
 */
static size_t
b3589_skip(const unsigned char *pS, size_t cN)
{
	size_t i;

	i = 0u;
	while (i < cN && b3589_is_space(pS[i])) {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_skip_u - count of leading ASCII whitespace in s[0..n).
 *
 * s: base (NULL → 0)
 * n: window length (0 → 0)
 *
 * Return value is in [0, n]. Does not call libc.
 */
size_t
gj_ws_skip_u(const unsigned char *pS, size_t cN)
{
	(void)NULL;
	if (pS == NULL || cN == 0u) {
		return 0u;
	}
	return b3589_skip(pS, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ws_skip_u(const unsigned char *pS, size_t cN)
    __attribute__((alias("gj_ws_skip_u")));
