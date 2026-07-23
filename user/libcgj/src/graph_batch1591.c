/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1591: JSON whitespace predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_ws_u(int c);
 *     — Return 1 if c is JSON insignificant whitespace (RFC 8259 §2):
 *       space (0x20), tab (0x09), LF (0x0A), CR (0x0D). Else 0.
 *   int __gj_json_is_ws_u  (alias)
 *   __libcgj_batch1591_marker = "libcgj-batch1591"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1591_marker[] = "libcgj-batch1591";

/* ---- freestanding helpers ---------------------------------------------- */

/* JSON ws code points only (no form-feed / vertical-tab). */
static int
b1591_is_ws(int nC)
{
	return (nC == 0x20 || nC == 0x09 || nC == 0x0a || nC == 0x0d) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_ws_u — true for JSON insignificant whitespace.
 *
 * Other values (including EOF-style negatives) return 0.
 */
int
gj_json_is_ws_u(int nC)
{
	return b1591_is_ws(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_ws_u(int nC)
    __attribute__((alias("gj_json_is_ws_u")));
