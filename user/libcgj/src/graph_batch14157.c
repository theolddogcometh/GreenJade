/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14157: soft controller stick deadzone default.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctl_stick_dz_soft_14157(void);
 *     - Returns 8192 (soft default stick deadzone radius on a signed
 *       16-bit axis scale). Pure-data threshold stub; does not sample
 *       axes or open /dev/input.
 *   uint32_t __gj_ctl_stick_dz_soft_14157  (alias)
 *   __libcgj_batch14157_marker = "libcgj-batch14157"
 *
 * Exclusive continuum CREATE-ONLY (14151-14160: controller/input soft —
 * ctl_axis_lx_id_14151, ctl_axis_ly_id_14152, ctl_axis_rx_id_14153,
 * ctl_axis_ry_id_14154, ctl_btn_tl_id_14155, ctl_btn_tr_id_14156,
 * ctl_stick_dz_soft_14157, ctl_slot_max_soft_14158,
 * ctl_input_soft_ready_14159, batch_id_14160).
 * Unique surface only; no multi-def. Distinct from
 * gj_ctl_btn_tr_id_14156 (batch14156) and gj_axis_deadzone_apply
 * (batch1915). No parent wires. No __int128.
 * No HID/controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14157_marker[] = "libcgj-batch14157";

/* Soft default stick deadzone radius on s16 axis scale (~25% of 32767). */
#define B14157_STICK_DZ  8192u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14157_stick_dz(void)
{
	return B14157_STICK_DZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctl_stick_dz_soft_14157 - soft default stick deadzone radius.
 *
 * Always returns 8192. Soft continuum constant for freestanding axis
 * packing; does not read HID reports. No parent wires.
 */
uint32_t
gj_ctl_stick_dz_soft_14157(void)
{
	(void)NULL;
	return b14157_stick_dz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ctl_stick_dz_soft_14157(void)
    __attribute__((alias("gj_ctl_stick_dz_soft_14157")));
