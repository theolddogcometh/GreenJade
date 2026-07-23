/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5877: HID RD checksum fold stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_checksum_fold_u(uint32_t sum);
 *     - Folds a rolling HID RD sum to 16 bits (sum & 0xffff).
 *   uint32_t __gj_hid_rd_checksum_fold_u  (alias)
 *   __libcgj_batch5877_marker = "libcgj-batch5877"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_checksum_fold_u surface only;
 * no multi-def. Distinct from gj_ip_checksum_fold. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5877_marker[] = "libcgj-batch5877";

#define B5877_HID_RD_FOLD_MASK  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5877_checksum_fold(uint32_t sum)
{
	return sum & B5877_HID_RD_FOLD_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_checksum_fold_u - fold HID RD rolling checksum to 16 bits.
 *
 * Returns sum & 0xffff. Soft freestanding stub for RD integrity tags;
 * not ones-complement Internet fold. No parent wires.
 */
uint32_t
gj_hid_rd_checksum_fold_u(uint32_t sum)
{
	(void)NULL;
	return b5877_checksum_fold(sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_checksum_fold_u(uint32_t sum)
    __attribute__((alias("gj_hid_rd_checksum_fold_u")));
