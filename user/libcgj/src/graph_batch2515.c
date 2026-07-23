/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2515: JSON keyword / ident start predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_ident_start(int c);
 *     - Return 1 if c may start a JSON keyword (true / false / null) or
 *       a relaxed unquoted identifier: ASCII letter [A-Za-z] or '_'.
 *       Digits and other punctuation → 0. Values outside 0..255 → 0.
 *   int __gj_json_is_ident_start  (alias)
 *   __libcgj_batch2515_marker = "libcgj-batch2515"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_str_is_ident / gj_css_is_ident_start — JSON-named start-only
 * predicate; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2515_marker[] = "libcgj-batch2515";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if c is ASCII letter or underscore (JSON keyword / relaxed ident). */
static int
b2515_is_ident_start(int nC)
{
	if (nC < 0 || nC > 255) {
		return 0;
	}
	if ((nC >= (int)'A' && nC <= (int)'Z') ||
	    (nC >= (int)'a' && nC <= (int)'z')) {
		return 1;
	}
	if (nC == (int)'_') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_ident_start - true for first char of true/false/null (or
 * relaxed ident). Digits alone are not starts (JSON numbers use
 * gj_json_is_number_start).
 */
int
gj_json_is_ident_start(int nC)
{
	(void)NULL;
	return b2515_is_ident_start(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_ident_start(int nC)
    __attribute__((alias("gj_json_is_ident_start")));
