/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5330: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5330(void);
 *     - Returns the compile-time graph batch number for this TU (5330).
 *   uint32_t __gj_batch_id_5330  (alias)
 *   __libcgj_batch5330_marker = "libcgj-batch5330"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5330_marker[] = "libcgj-batch5330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5330_id(void)
{
	return 5330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5330 - report this TU's graph batch number.
 *
 * Always returns 5330. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5330(void)
{
	(void)NULL;
	return b5330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5330(void)
    __attribute__((alias("gj_batch_id_5330")));
