/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2984: skip JSON whitespace (bounded index, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_json_skip_ws_u(const char *s, size_t n, size_t i);
 *     - From index i in s[0..n), advance over JSON whitespace
 *       (space / tab / CR / LF per RFC 8259 §2). Returns the first
 *       non-ws index, or n if the remainder is all whitespace.
 *       NULL s → i (unchanged). i >= n → i.
 *   size_t __gj_json_skip_ws_u  (alias)
 *   __libcgj_batch2984_marker = "libcgj-batch2984"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_json_skip_ws (batch742 out-param) and gj_json_skip_ws_n
 * (batch1592) — unique gj_json_skip_ws_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2984_marker[] = "libcgj-batch2984";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is JSON insignificant whitespace. */
static int
b2984_is_ws(unsigned char uCh)
{
	return (uCh == 0x20u || uCh == 0x09u || uCh == 0x0au ||
	    uCh == 0x0du) ? 1 : 0;
}

/* Advance cI over ws in sz[0..cN). */
static size_t
b2984_skip_ws(const char *sz, size_t cN, size_t cI)
{
	while (cI < cN && b2984_is_ws((unsigned char)sz[cI]) != 0) {
		cI++;
	}
	return cI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_ws_u - return index after leading JSON ws from i in s[0..n).
 *
 * Pure index walk; does not write through any out-parameter.
 */
size_t
gj_json_skip_ws_u(const char *sz, size_t cN, size_t cI)
{
	(void)NULL;
	if (sz == NULL) {
		return cI;
	}
	if (cI >= cN) {
		return cI;
	}
	return b2984_skip_ws(sz, cN, cI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_skip_ws_u(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_skip_ws_u")));
