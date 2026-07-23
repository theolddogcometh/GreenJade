/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5709: bar3 checklist popcount (low nibble).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_popcount_5709(uint32_t mask);
 *     - Return the number of set bits among the low four product bar3
 *       checklist bits (steam/deck/top50/install), range [0, 4]. Soft
 *       pure-data popcount; higher bits of mask are ignored.
 *   uint32_t __gj_bar3_checklist_popcount_5709  (alias)
 *   __libcgj_batch5709_marker = "libcgj-batch5709"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_popcount_5709 surface only; no
 * multi-def. Distinct from gj_bar3_checklist_bits_ok_5706 (complete
 * gate) and clear/set bit helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5709_marker[] = "libcgj-batch5709";

/* Low four checklist bits only. */
#define B5709_CHECKLIST_MASK  0xFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5709_popcount(uint32_t uMask)
{
	uint32_t uNibble;
	uint32_t uCount;

	uNibble = uMask & B5709_CHECKLIST_MASK;
	uCount = 0u;
	while (uNibble != 0u) {
		uCount += uNibble & 1u;
		uNibble >>= 1u;
	}
	return uCount;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_popcount_5709 - count set checklist bits (0..4).
 *
 * mask: product bar3 install-checklist bitmask
 *
 * Returns popcount of bits 0..3. Soft pure-data; no libc. No parent
 * wires.
 */
uint32_t
gj_bar3_checklist_popcount_5709(uint32_t uMask)
{
	(void)NULL;
	return b5709_popcount(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_popcount_5709(uint32_t uMask)
    __attribute__((alias("gj_bar3_checklist_popcount_5709")));
