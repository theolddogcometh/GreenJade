/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5872: HID report descriptor maximum length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_len_max_u(void);
 *     - Returns the soft maximum HID report descriptor length (4096).
 *   uint32_t __gj_hid_rd_len_max_u  (alias)
 *   __libcgj_batch5872_marker = "libcgj-batch5872"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_len_max_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5872_marker[] = "libcgj-batch5872";

/* Soft maximum HID report descriptor byte length (host-side bound). */
#define B5872_HID_RD_LEN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5872_rd_len_max(void)
{
	return B5872_HID_RD_LEN_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_len_max_u - report the soft maximum HID report descriptor length.
 *
 * Always returns 4096. Soft compile-time constant for freestanding HID RD
 * length bounds; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_rd_len_max_u(void)
{
	(void)NULL;
	return b5872_rd_len_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_len_max_u(void)
    __attribute__((alias("gj_hid_rd_len_max_u")));
