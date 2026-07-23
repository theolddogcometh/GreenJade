/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8939: soft input/evdev id stub continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ready_u_8939(void);
 *     - Returns 1 (input/evdev soft id stub continuum ready). Pure-data
 *       product tag that exclusive wave 8931-8940 surfaces are present.
 *   uint32_t __gj_evdev_ready_u_8939  (alias)
 *   __libcgj_batch8939_marker = "libcgj-batch8939"
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

const char __libcgj_batch8939_marker[] = "libcgj-batch8939";

#define B8939_EVDEV_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8939_evdev_ready(void)
{
	return B8939_EVDEV_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ready_u_8939 - input/evdev soft id stub continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive wave
 * surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_evdev_ready_u_8939(void)
{
	(void)NULL;
	return b8939_evdev_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ready_u_8939(void)
    __attribute__((alias("gj_evdev_ready_u_8939")));
