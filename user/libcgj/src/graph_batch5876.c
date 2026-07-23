/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5876: HID RD checksum byte add stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_checksum_add_u(uint32_t acc, uint8_t b);
 *     - Soft rolling add: returns acc + b (no wrap policy beyond uint32).
 *   uint32_t __gj_hid_rd_checksum_add_u  (alias)
 *   __libcgj_batch5876_marker = "libcgj-batch5876"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_checksum_add_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5876_marker[] = "libcgj-batch5876";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5876_checksum_add(uint32_t acc, uint8_t b)
{
	return acc + (uint32_t)b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_checksum_add_u - soft HID RD rolling checksum step.
 *
 * Returns acc + b. Freestanding integrity stub for report descriptor
 * bytes; not a USB CRC-5/16. No parent wires.
 */
uint32_t
gj_hid_rd_checksum_add_u(uint32_t acc, uint8_t b)
{
	(void)NULL;
	return b5876_checksum_add(acc, b);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_checksum_add_u(uint32_t acc, uint8_t b)
    __attribute__((alias("gj_hid_rd_checksum_add_u")));
