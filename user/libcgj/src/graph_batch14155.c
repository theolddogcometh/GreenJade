/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14155: soft controller left-shoulder button id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctl_btn_tl_id_14155(void);
 *     - Returns 0x136 (Linux input BTN_TL soft catalog value for the
 *       left shoulder / L1 button). Pure-data button id stub; does not
 *       poll controllers or open /dev/input.
 *   uint32_t __gj_ctl_btn_tl_id_14155  (alias)
 *   __libcgj_batch14155_marker = "libcgj-batch14155"
 *
 * Exclusive continuum CREATE-ONLY (14151-14160: controller/input soft —
 * ctl_axis_lx_id_14151, ctl_axis_ly_id_14152, ctl_axis_rx_id_14153,
 * ctl_axis_ry_id_14154, ctl_btn_tl_id_14155, ctl_btn_tr_id_14156,
 * ctl_stick_dz_soft_14157, ctl_slot_max_soft_14158,
 * ctl_input_soft_ready_14159, batch_id_14160).
 * Unique surface only; no multi-def. Distinct from
 * gj_ctl_axis_ry_id_14154 (batch14154). No parent wires. No __int128.
 * No HID/controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14155_marker[] = "libcgj-batch14155";

/* Soft left-shoulder button id (Linux BTN_TL = 0x136). */
#define B14155_BTN_TL  0x136u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14155_btn_tl(void)
{
	return B14155_BTN_TL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctl_btn_tl_id_14155 - soft controller left-shoulder (L1) catalog id.
 *
 * Always returns 0x136 (BTN_TL). Soft continuum constant; does not poll
 * pads or call libinput/SDL. No parent wires.
 */
uint32_t
gj_ctl_btn_tl_id_14155(void)
{
	(void)NULL;
	return b14155_btn_tl();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ctl_btn_tl_id_14155(void)
    __attribute__((alias("gj_ctl_btn_tl_id_14155")));
