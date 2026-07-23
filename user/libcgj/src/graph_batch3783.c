/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3783: Manacher-style center expand step (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_manacher_expand_u(const unsigned char *s, size_t n,
 *                               size_t center, size_t radius);
 *     - Starting from a known palindrome radius around center in a
 *       separator-style or plain buffer s[0..n), expand while
 *         center >= radius+1 and center+radius+1 < n and
 *         s[center-radius-1] == s[center+radius+1].
 *       Returns the final radius (may equal the input radius if no
 *       further expansion). NULL s, n == 0, or center >= n → 0.
 *       If center < radius (inconsistent seed), returns 0.
 *   size_t __gj_manacher_expand_u  (alias)
 *   __libcgj_batch3783_marker = "libcgj-batch3783"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_manacher_len (batch238 full scan) — single expand step only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3783_marker[] = "libcgj-batch3783";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manacher_expand_u — grow a centered palindrome radius by matching
 * mirrored characters one step at a time.
 *
 * Does not allocate. No parent wires.
 */
size_t
gj_manacher_expand_u(const unsigned char *s, size_t n, size_t center,
                     size_t radius)
{
	(void)NULL;
	if (s == NULL || n == 0u || center >= n) {
		return 0u;
	}
	if (center < radius) {
		return 0u;
	}

	while (center >= radius + 1u && center + radius + 1u < n &&
	       s[center - radius - 1u] == s[center + radius + 1u]) {
		radius++;
	}
	return radius;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_manacher_expand_u(const unsigned char *s, size_t n,
                              size_t center, size_t radius)
    __attribute__((alias("gj_manacher_expand_u")));
