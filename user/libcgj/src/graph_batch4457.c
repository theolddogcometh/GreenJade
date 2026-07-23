/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4457: all-set test over nbytes of uint8_t.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_all_byte_u(const uint8_t *bits, size_t nbytes);
 *     - Return 1 if every bit is set across nbytes full bytes (each
 *       byte == 0xFF), else 0. NULL bits → 0. nbytes == 0 → 1
 *       (vacuous truth over an empty range).
 *   int __gj_bitmap_all_byte_u  (alias)
 *   __libcgj_batch4457_marker = "libcgj-batch4457"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Unique gj_bitmap_all_byte_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4457_marker[] = "libcgj-batch4457";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if every byte in [0, cBytes) is 0xFF. */
static int
b4457_all(const uint8_t *pBits, size_t cBytes)
{
	size_t iByte;

	for (iByte = 0u; iByte < cBytes; iByte++) {
		if (pBits[iByte] != 0xFFu) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_all_byte_u - 1 if all bits are set over nbytes bytes.
 *
 * bits:   dense byte bitmap (may be NULL → 0)
 * nbytes: number of full bytes to scan; 0 → vacuous 1
 *
 * Returns 1 if every bit is set (or nbytes is 0), else 0. No libc.
 */
int
gj_bitmap_all_byte_u(const uint8_t *pBits, size_t cBytes)
{
	if (pBits == NULL) {
		return 0;
	}
	if (cBytes == 0u) {
		return 1;
	}
	return b4457_all(pBits, cBytes);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_all_byte_u(const uint8_t *pBits, size_t cBytes)
    __attribute__((alias("gj_bitmap_all_byte_u")));
