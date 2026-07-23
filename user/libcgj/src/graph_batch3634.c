/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3634: two-string join buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_join_need_u(const char *a, const char *b);
 *     - Bytes needed (including trailing NUL) for the concatenation that
 *       gj_str_join2 (batch681) would write for (a, b) — a then b with no
 *       separator. NULL a or b yields 0. Empty a → len(b)+1; empty b →
 *       len(a)+1; both empty → 1 (NUL only).
 *   size_t __gj_str_join_need_u  (alias)
 *   __libcgj_batch3634_marker = "libcgj-batch3634"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_join2 (batch681 write), gj_path_join_need_u / gj_path_join2_need_u
 * (path '/' forms) — unique plain-join length surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3634_marker[] = "libcgj-batch3634";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3634_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Bytes needed (incl. trailing NUL) for join2(a,b). NULL either → 0.
 * Overflow of size_t addition → 0.
 */
static size_t
b3634_join_need(const char *szA, const char *szB)
{
	size_t cbA;
	size_t cbB;
	size_t nOut;

	if (szA == NULL || szB == NULL) {
		return 0u;
	}

	cbA = b3634_len(szA);
	cbB = b3634_len(szB);

	/* nOut = cbA + cbB + 1 (NUL); refuse wrap */
	if (cbA > SIZE_MAX - 1u) {
		return 0u;
	}
	nOut = cbA + 1u;
	if (cbB > SIZE_MAX - nOut) {
		return 0u;
	}
	return nOut + cbB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join_need_u - bytes needed (incl. NUL) for plain join of a and b.
 *
 * a, b: C string components (NULL either → 0)
 *
 * Examples (return includes trailing NUL):
 *   ("a","b")  → 3  ("ab" + NUL)
 *   ("","")    → 1  ("" + NUL)
 *   ("hi","")  → 3  ("hi" + NUL)
 *   (NULL,"x") → 0
 *
 * Length-only; does not write a buffer. Does not call libc.
 * No parent wires.
 */
size_t
gj_str_join_need_u(const char *a, const char *b)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3634_join_need(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_join_need_u(const char *a, const char *b)
    __attribute__((alias("gj_str_join_need_u")));
