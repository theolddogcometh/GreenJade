/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch306: byte histogram into 256 bins.
 *
 * Surface (unique symbols):
 *   void gj_hist_u8(const uint8_t *data, size_t n, uint32_t bins[256]);
 *     — Zero bins[0..255], then count occurrences of each byte value in
 *       data[0..n). bins[v] becomes the number of times byte v appears.
 *       data may be NULL when n == 0 (empty → all bins zero).
 *       NULL bins is a no-op. NULL data with n > 0 is treated as empty
 *       (bins zeroed) to avoid deref — no errno in freestanding.
 *   __gj_hist_u8  (alias)
 *   __libcgj_batch306_marker = "libcgj-batch306"
 *
 * Distinct from batch210 (radix sort stack hist) which does not export
 * a public byte-histogram API.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch306_marker[] = "libcgj-batch306";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hist_u8 — count byte values into bins[256].
 *
 * data: input octets (NULL only meaningful when n == 0)
 * n:    number of bytes to scan
 * bins: destination histogram; each entry is a frequency count (uint32_t).
 *       Caller supplies storage for exactly 256 bins. On return,
 *       sum(bins[0..255]) == n when data was non-NULL (or n == 0).
 *
 * Edge cases:
 *   bins == NULL              → no-op
 *   n == 0 or data == NULL    → bins zeroed (empty histogram)
 *   n > 0 and data non-NULL   → full scan and accumulate
 *
 * Note: if n > UINT32_MAX for a single byte value, the bin wraps modulo
 * 2^32 (unsigned). Typical buffers are far below that limit.
 */
void
gj_hist_u8(const uint8_t *data, size_t n, uint32_t bins[256])
{
	size_t i;

	if (bins == NULL) {
		return;
	}

	for (i = 0u; i < 256u; i++) {
		bins[i] = 0u;
	}

	if (data == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		bins[data[i]]++;
	}
}

void __gj_hist_u8(const uint8_t *data, size_t n, uint32_t bins[256])
    __attribute__((alias("gj_hist_u8")));
