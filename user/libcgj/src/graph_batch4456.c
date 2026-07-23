/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4456: any-set test over nbytes of uint8_t.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_any_byte_u(const uint8_t *bits, size_t nbytes);
 *     - Return 1 if any bit is set across nbytes full bytes, else 0.
 *       NULL bits or nbytes == 0 → 0.
 *   int __gj_bitmap_any_byte_u  (alias)
 *   __libcgj_batch4456_marker = "libcgj-batch4456"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Distinct from gj_bitmap_test_range_any (batch1053) and word-bitmap
 * scans (batch2851+). Unique gj_bitmap_any_byte_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4456_marker[] = "libcgj-batch4456";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if any non-zero byte in [0, cBytes). */
static int
b4456_any(const uint8_t *pBits, size_t cBytes)
{
	size_t iByte;

	for (iByte = 0u; iByte < cBytes; iByte++) {
		if (pBits[iByte] != 0u) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_any_byte_u - 1 if any set bit exists over nbytes bytes.
 *
 * bits:   dense byte bitmap (may be NULL → 0)
 * nbytes: number of full bytes to scan
 *
 * Returns 1 if any bit is set, else 0 (including NULL / empty). No libc.
 */
int
gj_bitmap_any_byte_u(const uint8_t *pBits, size_t cBytes)
{
	if (pBits == NULL || cBytes == 0u) {
		return 0;
	}
	return b4456_any(pBits, cBytes);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_any_byte_u(const uint8_t *pBits, size_t cBytes)
    __attribute__((alias("gj_bitmap_any_byte_u")));
