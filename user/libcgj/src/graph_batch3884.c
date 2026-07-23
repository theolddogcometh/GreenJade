/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3884: JSON nesting depth delta for one char (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_depth_delta_u(int c);
 *     - Return nesting delta for a single structural character:
 *         '{' / '[' → +1
 *         '}' / ']' → -1
 *         all other values → 0
 *       Used by streaming scanners to track object/array depth without
 *       a full parse.
 *   int __gj_json_depth_delta_u  (alias)
 *   __libcgj_batch3884_marker = "libcgj-batch3884"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_json_depth_delta (batch2518) — unique _u surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3884_marker[] = "libcgj-batch3884";

/* ---- freestanding helpers ---------------------------------------------- */

/* Nesting delta for one code unit (open +1, close -1, else 0). */
static int
b3884_depth_delta(int nC)
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
 * gj_json_depth_delta_u — nesting change for one JSON structural char.
 *
 * c: character / code unit (typically unsigned char promoted to int)
 *
 * Does not interpret characters inside strings; caller must only feed
 * structural positions. Does not call libc. No parent wires.
 */
int
gj_json_depth_delta_u(int nC)
{
	(void)NULL;
	return b3884_depth_delta(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_depth_delta_u(int nC)
    __attribute__((alias("gj_json_depth_delta_u")));
