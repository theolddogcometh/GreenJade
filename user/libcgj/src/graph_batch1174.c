/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1174: RFC 4648 §5 base64url alphabet predicate.
 *
 * Surface (unique symbols):
 *   int gj_base64url_char_ok(int c);
 *     — Return 1 if c is in the base64url alphabet A-Za-z0-9-_
 *       or optional padding '='; else 0. Does not accept '+'/'.'.
 *   int __gj_base64url_char_ok  (alias)
 *   __libcgj_batch1174_marker = "libcgj-batch1174"
 *
 * Distinct from gj_base64url_enc/dec (batch551/552) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1174_marker[] = "libcgj-batch1174";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64url_char_ok — 1 iff c is base64url alphabet or '='.
 */
int
gj_base64url_char_ok(int c)
{
	if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	if (c >= '0' && c <= '9') {
		return 1;
	}
	if (c == '-' || c == '_' || c == '=') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base64url_char_ok(int c)
    __attribute__((alias("gj_base64url_char_ok")));
