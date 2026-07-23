/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14158: soft controller player-slot max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctl_slot_max_soft_14158(void);
 *     - Returns 7 (inclusive soft max local controller / player slot
 *       index for eight soft slots [0, 7]). Pure-data bound stub; does
 *       not enumerate pads or open /dev/input.
 *   uint32_t __gj_ctl_slot_max_soft_14158  (alias)
 *   __libcgj_batch14158_marker = "libcgj-batch14158"
 *
 * Exclusive continuum CREATE-ONLY (14151-14160: controller/input soft —
 * ctl_axis_lx_id_14151, ctl_axis_ly_id_14152, ctl_axis_rx_id_14153,
 * ctl_axis_ry_id_14154, ctl_btn_tl_id_14155, ctl_btn_tr_id_14156,
 * ctl_stick_dz_soft_14157, ctl_slot_max_soft_14158,
 * ctl_input_soft_ready_14159, batch_id_14160).
 * Unique surface only; no multi-def. Distinct from
 * gj_controller_slot_ok_u (batch2279) and gj_ctl_stick_dz_soft_14157
 * (batch14157). No parent wires. No __int128.
 * No HID/controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14158_marker[] = "libcgj-batch14158";

/* Inclusive soft max local controller / player slot (eight slots). */
#define B14158_SLOT_MAX  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14158_slot_max(void)
{
	return B14158_SLOT_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctl_slot_max_soft_14158 - soft max local controller slot index.
 *
 * Always returns 7 (slots [0, 7]). Soft continuum constant; does not
 * enumerate pads or call libinput/SDL. No parent wires.
 */
uint32_t
gj_ctl_slot_max_soft_14158(void)
{
	(void)NULL;
	return b14158_slot_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ctl_slot_max_soft_14158(void)
    __attribute__((alias("gj_ctl_slot_max_soft_14158")));
