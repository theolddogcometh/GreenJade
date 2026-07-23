/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5875: HID RD checksum seed (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_checksum_seed_u(void);
 *     - Returns the soft rolling-checksum seed for HID RD bytes (0).
 *   uint32_t __gj_hid_rd_checksum_seed_u  (alias)
 *   __libcgj_batch5875_marker = "libcgj-batch5875"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_checksum_seed_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5875_marker[] = "libcgj-batch5875";

/* Soft seed for freestanding HID report descriptor rolling checksum. */
#define B5875_HID_RD_CHECKSUM_SEED  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5875_checksum_seed(void)
{
	return B5875_HID_RD_CHECKSUM_SEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_checksum_seed_u - report the HID RD checksum seed.
 *
 * Always returns 0. Soft compile-time constant for freestanding HID RD
 * integrity stubs; not a USB-spec CRC. No parent wires.
 */
uint32_t
gj_hid_rd_checksum_seed_u(void)
{
	(void)NULL;
	return b5875_checksum_seed();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_checksum_seed_u(void)
    __attribute__((alias("gj_hid_rd_checksum_seed_u")));
