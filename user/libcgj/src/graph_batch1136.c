/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1136: JSON character needs-escape predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_needs_escape(int ch);
 *     — Return 1 if ch must be escaped inside a JSON string:
 *       '"' / '\\' / controls 0x00..0x1f. ch outside 0..255 → 1
 *       (conservative: cannot emit raw). Printable non-special → 0.
 *   int __gj_json_needs_escape  (alias)
 *   __libcgj_batch1136_marker = "libcgj-batch1136"
 *
 * Distinct from gj_json_escape_len — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1136_marker[] = "libcgj-batch1136";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_needs_escape — 1 if nCh requires escaping in a JSON string.
 */
int
gj_json_needs_escape(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 1;
	}
	if (nCh == '"' || nCh == '\\') {
		return 1;
	}
	if (nCh < 0x20) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_needs_escape(int nCh)
    __attribute__((alias("gj_json_needs_escape")));
