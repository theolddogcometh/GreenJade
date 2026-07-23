/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2071: URL percent-encode needs predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_url_needs_encode_p(int c);
 *     — Return 1 if octet c must be percent-encoded under RFC 3986
 *       unreserved rules (ALPHA / DIGIT / "-" / "." / "_" / "~" pass
 *       through; all other octets need %HH). c outside 0..255 → 1
 *       (conservative: cannot emit raw). Unreserved → 0.
 *   int __gj_str_url_needs_encode_p  (alias)
 *   __libcgj_batch2071_marker = "libcgj-batch2071"
 *
 * Str encode exclusive wave (2071–2080). Distinct from gj_url_encode /
 * gj_pct_encode — predicate only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2071_marker[] = "libcgj-batch2071";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2071_is_unreserved — RFC 3986 §2.3 unreserved set:
 *   ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
static int
b2071_is_unreserved(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C == (unsigned char)'-' || u8C == (unsigned char)'.' ||
	    u8C == (unsigned char)'_' || u8C == (unsigned char)'~') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_url_needs_encode_p — 1 if c requires RFC 3986 percent-encoding.
 *
 * c: candidate octet (or wider int; outside 0..255 treated as needing encode)
 *
 * Does not call libc.
 */
int
gj_str_url_needs_encode_p(int nC)
{
	(void)NULL;
	if (nC < 0 || nC > 255) {
		return 1;
	}
	if (b2071_is_unreserved((unsigned char)nC)) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_url_needs_encode_p(int nC)
    __attribute__((alias("gj_str_url_needs_encode_p")));
