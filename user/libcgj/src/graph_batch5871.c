/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5871: HID report descriptor minimum length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_len_min_u(void);
 *     - Returns the soft minimum valid HID report descriptor length (1).
 *   uint32_t __gj_hid_rd_len_min_u  (alias)
 *   __libcgj_batch5871_marker = "libcgj-batch5871"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs — hid_rd_len_min_u, hid_rd_len_max_u,
 * hid_rd_short_item_hdr_u, hid_rd_long_item_hdr_u, hid_rd_checksum_seed_u,
 * hid_rd_checksum_add_u, hid_rd_checksum_fold_u, hid_rd_len_keyboard_u,
 * hid_rd_len_mouse_u, batch_id_5880). Unique gj_hid_rd_len_min_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5871_marker[] = "libcgj-batch5871";

/* Soft minimum HID report descriptor byte length. */
#define B5871_HID_RD_LEN_MIN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5871_rd_len_min(void)
{
	return B5871_HID_RD_LEN_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_len_min_u - report the soft minimum HID report descriptor length.
 *
 * Always returns 1. Soft compile-time constant for freestanding HID RD
 * length bounds; does not parse descriptors. No parent wires.
 */
uint32_t
gj_hid_rd_len_min_u(void)
{
	(void)NULL;
	return b5871_rd_len_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_len_min_u(void)
    __attribute__((alias("gj_hid_rd_len_min_u")));
