/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1122: HTTP OWS whitespace predicate.
 *
 * Surface (unique symbols):
 *   int gj_http_is_ws(int ch);
 *     — Return 1 if ch is HTTP whitespace: SP (0x20) or HTAB (0x09)
 *       per RFC 7230 OWS/RWS/BWS. Other values (incl. CR/LF) → 0.
 *       ch outside 0..255 → 0.
 *   int __gj_http_is_ws  (alias)
 *   __libcgj_batch1122_marker = "libcgj-batch1122"
 *
 * Distinct from isspace / internal b*_is_ws helpers — unique public name.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1122_marker[] = "libcgj-batch1122";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_is_ws — 1 if nCh is SP or HTAB (HTTP OWS).
 */
int
gj_http_is_ws(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	return (nCh == 0x20 || nCh == 0x09) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_http_is_ws(int nCh)
    __attribute__((alias("gj_http_is_ws")));
