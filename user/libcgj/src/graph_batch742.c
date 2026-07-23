/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch742: advance index over JSON whitespace.
 *
 * Surface (unique symbols):
 *   int gj_json_skip_ws(const char *s, size_t n, size_t *i);
 *     — While *i < n and s[*i] is JSON ws (space/tab/CR/LF), advance *i.
 *       Always returns 0. NULL s or NULL i is a no-op (still returns 0).
 *   int __gj_json_skip_ws  (alias)
 *   __libcgj_batch742_marker = "libcgj-batch742"
 *
 * Does NOT redefine gj_json_is_ws (batch741) or other gj_json_* surfaces —
 * avoid multi-def. Whitespace test is inlined (same four code points).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch742_marker[] = "libcgj-batch742";

/* ---- helpers ----------------------------------------------------------- */

static int
b742_is_ws(int c)
{
	return (c == 0x20 || c == 0x09 || c == 0x0a || c == 0x0d) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_skip_ws — advance *i over leading JSON whitespace in s[0..n).
 *
 * s: buffer of n bytes (may be NULL only when treated as empty)
 * n: length of s
 * i: in/out cursor; must be non-NULL to advance
 *
 * Returns 0 always. Does not clamp *i if it already exceeds n.
 */
int
gj_json_skip_ws(const char *s, size_t n, size_t *i)
{
	if (s == NULL || i == NULL) {
		return 0;
	}
	while (*i < n && b742_is_ws((unsigned char)s[*i])) {
		(*i)++;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_skip_ws(const char *s, size_t n, size_t *i)
    __attribute__((alias("gj_json_skip_ws")));
