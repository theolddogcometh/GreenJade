/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14160: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14160(void);
 *     - Returns the compile-time graph batch number for this TU (14160).
 *   uint32_t __gj_batch_id_14160  (alias)
 *   __libcgj_batch14160_marker = "libcgj-batch14160"
 *
 * Exclusive continuum CREATE-ONLY (14151-14160: controller/input soft —
 * ctl_axis_lx_id_14151, ctl_axis_ly_id_14152, ctl_axis_rx_id_14153,
 * ctl_axis_ry_id_14154, ctl_btn_tl_id_14155, ctl_btn_tr_id_14156,
 * ctl_stick_dz_soft_14157, ctl_slot_max_soft_14158,
 * ctl_input_soft_ready_14159, batch_id_14160).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14160_marker[] = "libcgj-batch14160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14160_id(void)
{
	return 14160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14160 - report this TU's graph batch number.
 *
 * Always returns 14160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_14160(void)
{
	(void)NULL;
	return b14160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14160(void)
    __attribute__((alias("gj_batch_id_14160")));
