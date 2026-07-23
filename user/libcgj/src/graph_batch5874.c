/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5874: HID RD long-item header size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_long_item_hdr_u(void);
 *     - Returns the HID long-item header size in bytes (3).
 *   uint32_t __gj_hid_rd_long_item_hdr_u  (alias)
 *   __libcgj_batch5874_marker = "libcgj-batch5874"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_long_item_hdr_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5874_marker[] = "libcgj-batch5874";

/*
 * HID long item header: prefix (0xFE), bDataSize, bLongItemTag
 * before optional data payload.
 */
#define B5874_HID_RD_LONG_ITEM_HDR  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5874_long_item_hdr(void)
{
	return B5874_HID_RD_LONG_ITEM_HDR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_long_item_hdr_u - report HID long-item header size.
 *
 * Always returns 3. Soft compile-time constant for freestanding HID RD
 * length accounting; does not parse streams. No parent wires.
 */
uint32_t
gj_hid_rd_long_item_hdr_u(void)
{
	(void)NULL;
	return b5874_long_item_hdr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_long_item_hdr_u(void)
    __attribute__((alias("gj_hid_rd_long_item_hdr_u")));
