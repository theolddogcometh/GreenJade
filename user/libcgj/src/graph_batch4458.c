/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4458: none-set test over nbytes of uint8_t.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_none_byte_u(const uint8_t *bits, size_t nbytes);
 *     - Return 1 if no bit is set across nbytes full bytes, else 0.
 *       NULL bits or nbytes == 0 → 1 (no set bits observed).
 *   int __gj_bitmap_none_byte_u  (alias)
 *   __libcgj_batch4458_marker = "libcgj-batch4458"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Complement of gj_bitmap_any_byte_u (batch4456). Unique
 * gj_bitmap_none_byte_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4458_marker[] = "libcgj-batch4458";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if every byte in [0, cBytes) is 0. */
static int
b4458_none(const uint8_t *pBits, size_t cBytes)
{
	size_t iByte;

	for (iByte = 0u; iByte < cBytes; iByte++) {
		if (pBits[iByte] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_none_byte_u - 1 if no set bits exist over nbytes bytes.
 *
 * bits:   dense byte bitmap (may be NULL → 1)
 * nbytes: number of full bytes to scan; 0 → 1
 *
 * Returns 1 if none are set (including NULL / empty), else 0. No libc.
 */
int
gj_bitmap_none_byte_u(const uint8_t *pBits, size_t cBytes)
{
	if (pBits == NULL || cBytes == 0u) {
		return 1;
	}
	return b4458_none(pBits, cBytes);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_none_byte_u(const uint8_t *pBits, size_t cBytes)
    __attribute__((alias("gj_bitmap_none_byte_u")));
