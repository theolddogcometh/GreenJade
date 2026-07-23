/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8931: soft Linux evdev EV_SYN type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_syn_id_8931(void);
 *     - Returns 0x00 (classic Linux EV_SYN input event type id).
 *       Soft catalog constant only; no input device I/O.
 *   uint32_t __gj_ev_syn_id_8931  (alias)
 *   __libcgj_batch8931_marker = "libcgj-batch8931"
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

const char __libcgj_batch8931_marker[] = "libcgj-batch8931";

/* Soft Linux EV_SYN event type catalog id. */
#define B8931_EV_SYN_ID  0x00u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8931_ev_syn_id(void)
{
	return B8931_EV_SYN_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_syn_id_8931 - soft Linux EV_SYN input event type catalog id.
 *
 * Always returns 0x00. Soft continuum constant; does not open devices
 * or read events. No parent wires.
 */
uint32_t
gj_ev_syn_id_8931(void)
{
	(void)NULL;
	return b8931_ev_syn_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_syn_id_8931(void)
    __attribute__((alias("gj_ev_syn_id_8931")));
