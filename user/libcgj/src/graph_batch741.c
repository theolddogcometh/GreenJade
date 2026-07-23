/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch741: JSON whitespace predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_ws(int c);
 *     — Return 1 if c is JSON whitespace (space / tab / CR / LF), else 0.
 *   int __gj_json_is_ws  (alias)
 *   __libcgj_batch741_marker = "libcgj-batch741"
 *
 * Does NOT redefine gj_json_get_* / json_escape / gj_json_pointer_get
 * (batch77 / 107 / 161 / 271 / 272) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch741_marker[] = "libcgj-batch741";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_ws — true for JSON insignificant whitespace (RFC 8259 §2):
 *   0x20 space, 0x09 tab, 0x0A LF, 0x0D CR.
 *
 * Other values (including EOF-style negatives) return 0.
 */
int
gj_json_is_ws(int c)
{
	return (c == 0x20 || c == 0x09 || c == 0x0a || c == 0x0d) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_ws(int c)
    __attribute__((alias("gj_json_is_ws")));
