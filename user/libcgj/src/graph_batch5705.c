/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5705: bar3 checklist install bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_install_bit_5705(uint32_t mask);
 *     - Return the install checklist bit of product bar3 mask
 *       ((mask & 0x8) != 0) as 0 or 1. Soft pure-data extract for the
 *       install-image product-bar slot.
 *   uint32_t __gj_bar3_checklist_install_bit_5705  (alias)
 *   __libcgj_batch5705_marker = "libcgj-batch5705"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_install_bit_5705 surface only; no
 * multi-def. Distinct from gj_bar3_install_bit (batch2671) and
 * gj_bar3_checklist_top50_bit_5704. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5705_marker[] = "libcgj-batch5705";

/* Install readiness bit within product bar3 checklist (bit3). */
#define B5705_INSTALL_BIT  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5705_install_bit(uint32_t uMask)
{
	if ((uMask & B5705_INSTALL_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_install_bit_5705 - extract install checklist bit.
 *
 * mask: product bar3 install-checklist bitmask (bit3 = install slot)
 *
 * Returns 1 when bit3 is set, else 0. Soft pure-data extract; not a
 * runtime ESP probe. No parent wires.
 */
uint32_t
gj_bar3_checklist_install_bit_5705(uint32_t uMask)
{
	(void)NULL;
	return b5705_install_bit(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_install_bit_5705(uint32_t uMask)
    __attribute__((alias("gj_bar3_checklist_install_bit_5705")));
