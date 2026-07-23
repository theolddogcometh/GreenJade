/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1592: skip JSON whitespace (bounded index).
 *
 * Surface (unique symbols):
 *   size_t gj_json_skip_ws_n(const char *s, size_t n, size_t i);
 *     — From index i in s[0..n), advance over JSON whitespace
 *       (space / tab / CR / LF). Returns the first non-ws index, or n
 *       if the remainder is all whitespace. NULL s or i >= n → i (or 0
 *       if s is NULL and i is 0).
 *   size_t __gj_json_skip_ws_n  (alias)
 *   __libcgj_batch1592_marker = "libcgj-batch1592"
 *
 * Distinct from gj_json_skip_ws (batch742 out-param form) — unique name.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1592_marker[] = "libcgj-batch1592";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1592_is_ws(unsigned char uCh)
{
	return (uCh == 0x20u || uCh == 0x09u || uCh == 0x0au ||
	    uCh == 0x0du) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_ws_n — return index after leading JSON ws from i in s[0..n).
 *
 * Pure; does not write through pointers other than reading s.
 */
size_t
gj_json_skip_ws_n(const char *sz, size_t cN, size_t cI)
{
	if (sz == NULL) {
		return cI;
	}
	if (cI >= cN) {
		return cI;
	}
	while (cI < cN && b1592_is_ws((unsigned char)sz[cI])) {
		cI++;
	}
	return cI;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_skip_ws_n(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_skip_ws_n")));
