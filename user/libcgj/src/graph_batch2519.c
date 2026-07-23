/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2519: JSON structural character predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_structural(int c);
 *     - Return 1 if c is a JSON structural character (RFC 8259 §2):
 *         '{'  '}'  '['  ']'  ':'  ','
 *       Else 0 (including whitespace and value-start characters).
 *   int __gj_json_is_structural  (alias)
 *   __libcgj_batch2519_marker = "libcgj-batch2519"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_depth_delta (batch2518 signed delta) — boolean only; avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2519_marker[] = "libcgj-batch2519";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if c is one of { } [ ] : , */
static int
b2519_is_structural(int nC)
{
	if (nC == (int)'{' || nC == (int)'}' ||
	    nC == (int)'[' || nC == (int)']' ||
	    nC == (int)':' || nC == (int)',') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_structural - true for JSON structural punctuation.
 *
 * Pair with gj_json_depth_delta for open/close and this predicate for
 * colon/comma recognition. Does not call libc.
 */
int
gj_json_is_structural(int nC)
{
	(void)NULL;
	return b2519_is_structural(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_structural(int nC)
    __attribute__((alias("gj_json_is_structural")));
