/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2514: JSON number start predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_number_start(int c);
 *     - Return 1 if c may begin an RFC 8259 number token: ASCII '-'
 *       or decimal digit '0'..'9'. Else 0 (including EOF-style negatives
 *       outside those code points).
 *   int __gj_json_is_number_start  (alias)
 *   __libcgj_batch2514_marker = "libcgj-batch2514"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_is_digit / gj_json_is_digit_u (digit-only) — includes leading
 * minus; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2514_marker[] = "libcgj-batch2514";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if c is '-' or ASCII decimal digit. */
static int
b2514_is_num_start(int nC)
{
	if (nC == (int)'-') {
		return 1;
	}
	if (nC >= (int)'0' && nC <= (int)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_number_start - true for first character of a JSON number.
 *
 * Matches RFC 8259 number production start: optional minus is itself a
 * start when followed by digits (caller validates the rest).
 */
int
gj_json_is_number_start(int nC)
{
	(void)NULL;
	return b2514_is_num_start(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_number_start(int nC)
    __attribute__((alias("gj_json_is_number_start")));
