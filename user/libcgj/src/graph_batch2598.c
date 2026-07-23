/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2598: install readiness bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_ready_bits(uint32_t mask);
 *     - Return the low four install readiness bits of mask
 *       (mask & 0xf). Soft pure-data extract for install-image /
 *       ESP / rootfs / kernel product slots; not a runtime probe.
 *   uint32_t __gj_install_ready_bits  (alias)
 *   __libcgj_batch2598_marker = "libcgj-batch2598"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_install_ready_bits surface only; no multi-def. Distinct from
 * gj_product_bar3_bits (batch2398 three-bit) and gj_product_bar3_mask
 * (batch2597 constant).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2598_marker[] = "libcgj-batch2598";

/* Four install readiness bits (bits 0..3). */
#define B2598_INSTALL_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract install readiness bits from mask. */
static uint32_t
b2598_bits(uint32_t uMask)
{
	return uMask & B2598_INSTALL_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_ready_bits - extract low four install readiness bits.
 *
 * mask: install readiness bitmask
 *
 * Returns mask & 0xf. Soft pure-data only; does not inspect ESP or
 * install images. Does not call libc.
 */
uint32_t
gj_install_ready_bits(uint32_t uMask)
{
	(void)NULL;
	return b2598_bits(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_ready_bits(uint32_t uMask)
    __attribute__((alias("gj_install_ready_bits")));
