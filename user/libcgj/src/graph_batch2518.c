/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2518: JSON nesting depth delta for one char.
 *
 * Surface (unique symbols):
 *   int gj_json_depth_delta(int c);
 *     - Return nesting delta for a single structural character:
 *         '{' / '[' → +1
 *         '}' / ']' → -1
 *         all other values → 0
 *       Used by streaming scanners to track object/array depth without
 *       a full parse.
 *   int __gj_json_depth_delta  (alias)
 *   __libcgj_batch2518_marker = "libcgj-batch2518"
 *
 * JSON exclusive lightweight wave (2511-2520). Unique surface; does not
 * redefine structural predicates — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2518_marker[] = "libcgj-batch2518";

/* ---- freestanding helpers ---------------------------------------------- */

/* Nesting delta for one code unit (open +1, close -1, else 0). */
static int
b2518_depth_delta(int nC)
{
	if (nC == (int)'{' || nC == (int)'[') {
		return 1;
	}
	if (nC == (int)'}' || nC == (int)']') {
		return -1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_depth_delta - nesting change for one JSON structural char.
 *
 * c: character / code unit (typically unsigned char promoted to int)
 *
 * Does not interpret characters inside strings; caller must only feed
 * structural positions. Does not call libc.
 */
int
gj_json_depth_delta(int nC)
{
	(void)NULL;
	return b2518_depth_delta(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_depth_delta(int nC)
    __attribute__((alias("gj_json_depth_delta")));
