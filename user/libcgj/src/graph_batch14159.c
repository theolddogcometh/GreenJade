/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14159: soft controller/input continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctl_input_soft_ready_14159(void);
 *     - Returns 1 (controller/input soft continuum ready). Pure-data
 *       product tag that exclusive wave 14151-14160 surfaces are present.
 *   uint32_t __gj_ctl_input_soft_ready_14159  (alias)
 *   __libcgj_batch14159_marker = "libcgj-batch14159"
 *
 * Exclusive continuum CREATE-ONLY (14151-14160: controller/input soft —
 * ctl_axis_lx_id_14151, ctl_axis_ly_id_14152, ctl_axis_rx_id_14153,
 * ctl_axis_ry_id_14154, ctl_btn_tl_id_14155, ctl_btn_tr_id_14156,
 * ctl_stick_dz_soft_14157, ctl_slot_max_soft_14158,
 * ctl_input_soft_ready_14159, batch_id_14160).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No HID/controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14159_marker[] = "libcgj-batch14159";

#define B14159_CTL_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14159_ready(void)
{
	return B14159_CTL_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctl_input_soft_ready_14159 - controller/input soft continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive wave
 * surfaces are present. Does not call libinput/SDL or libc. No parent
 * wires.
 */
uint32_t
gj_ctl_input_soft_ready_14159(void)
{
	(void)NULL;
	return b14159_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ctl_input_soft_ready_14159(void)
    __attribute__((alias("gj_ctl_input_soft_ready_14159")));
