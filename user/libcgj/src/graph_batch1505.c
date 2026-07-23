/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1505: longest LZ77 match length (capped).
 *
 * Surface (unique symbols):
 *   size_t gj_lz77_match_len(const uint8_t *a, const uint8_t *b,
 *                            size_t max);
 *     — Return the longest common prefix length of a[] and b[], capped
 *       at max. Stops at the first mismatch or when max bytes match.
 *       NULL a or NULL b → 0. max == 0 → 0.
 *   size_t __gj_lz77_match_len  (alias)
 *   __libcgj_batch1505_marker = "libcgj-batch1505"
 *
 * Caller must ensure a[0..max) and b[0..max) are readable when max > 0
 * and both pointers are non-NULL. Used with gj_lz77_hash3 (batch1504).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1505_marker[] = "libcgj-batch1505";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lz77_match_len — count matching bytes from a and b up to max.
 */
size_t
gj_lz77_match_len(const uint8_t *pA, const uint8_t *pB, size_t cbMax)
{
	size_t i;

	if (pA == NULL || pB == NULL || cbMax == 0u) {
		return 0u;
	}

	i = 0u;
	while (i < cbMax && pA[i] == pB[i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lz77_match_len(const uint8_t *pA, const uint8_t *pB,
    size_t cbMax)
    __attribute__((alias("gj_lz77_match_len")));
