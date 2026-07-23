/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5708: bar3 checklist clear bit by index.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_clear_bit_5708(uint32_t mask,
 *                                             uint32_t idx);
 *     - Return mask with checklist bit idx cleared, for idx in [0, 3]
 *       (0=steam, 1=deck, 2=top50, 3=install). Out-of-range idx leaves
 *       mask unchanged. Soft pure-data update.
 *   uint32_t __gj_bar3_checklist_clear_bit_5708  (alias)
 *   __libcgj_batch5708_marker = "libcgj-batch5708"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_clear_bit_5708 surface only; no
 * multi-def. Distinct from gj_bar3_checklist_set_bit_5707. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5708_marker[] = "libcgj-batch5708";

/* Valid checklist bit indices: 0..3. */
#define B5708_IDX_MAX  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5708_clear_bit(uint32_t uMask, uint32_t uIdx)
{
	if (uIdx > B5708_IDX_MAX) {
		return uMask;
	}
	return uMask & ~(1u << uIdx);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_clear_bit_5708 - clear one checklist bit by index.
 *
 * mask: product bar3 install-checklist bitmask
 * idx:  bit index 0=steam, 1=deck, 2=top50, 3=install
 *
 * Returns mask with that bit cleared, or unchanged mask if idx > 3.
 * Soft pure-data update; does not mutate storage. No parent wires.
 */
uint32_t
gj_bar3_checklist_clear_bit_5708(uint32_t uMask, uint32_t uIdx)
{
	(void)NULL;
	return b5708_clear_bit(uMask, uIdx);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_clear_bit_5708(uint32_t uMask, uint32_t uIdx)
    __attribute__((alias("gj_bar3_checklist_clear_bit_5708")));
