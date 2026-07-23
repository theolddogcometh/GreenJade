/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8936: soft Linux evdev EV_SW type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_sw_id_8936(void);
 *     - Returns 0x05 (classic Linux EV_SW input event type id).
 *       Soft catalog constant only; no input device I/O.
 *   uint32_t __gj_ev_sw_id_8936  (alias)
 *   __libcgj_batch8936_marker = "libcgj-batch8936"
 *
 * Exclusive continuum CREATE-ONLY (8931-8940: input/evdev id stubs —
 * ev_syn_id_8931, ev_key_id_8932, ev_rel_id_8933, ev_abs_id_8934,
 * ev_msc_id_8935, ev_sw_id_8936, ev_led_id_8937, ev_rep_id_8938,
 * evdev_ready_u_8939, batch_id_8940).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8936_marker[] = "libcgj-batch8936";

/* Soft Linux EV_SW event type catalog id. */
#define B8936_EV_SW_ID  0x05u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8936_ev_sw_id(void)
{
	return B8936_EV_SW_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_sw_id_8936 - soft Linux EV_SW input event type catalog id.
 *
 * Always returns 0x05. Soft continuum constant; does not open devices
 * or read switch events. No parent wires.
 */
uint32_t
gj_ev_sw_id_8936(void)
{
	(void)NULL;
	return b8936_ev_sw_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_sw_id_8936(void)
    __attribute__((alias("gj_ev_sw_id_8936")));
