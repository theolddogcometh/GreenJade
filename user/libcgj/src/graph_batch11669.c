/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11669: input soft stack ready lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_11669(void);
 *     - Returns 1 (input soft id stub surface considered ready for this
 *       continuum wave). Soft compile-time product tag; not a live
 *       input subsystem readiness probe.
 *   uint32_t __gj_input_soft_ready_u_11669  (alias)
 *   __libcgj_batch11669_marker = "libcgj-batch11669"
 *
 * Exclusive continuum CREATE-ONLY (11661-11670: input soft —
 * udev_ok_u_11661, libinput_ok_u_11662, evdev_ok_u_11663,
 * hidraw_ok_u_11664, uinput_ok_u_11665, hotplug_ok_u_11666,
 * device_db_ok_u_11667, hwdb_ok_u_11668, input_soft_ready_u_11669,
 * batch_id_11670). Unique surface only; no multi-def. Distinct from
 * gj_input_soft_ready_u_11469 / gj_input_soft_ready_u_11269 prior
 * waves. Ready lamp is the only on-lamp (1) in this wave; sibling ok
 * lamps return 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11669_marker[] = "libcgj-batch11669";

/* Soft lamp: input soft stack ready for this continuum. */
#define B11669_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11669_input_soft_ready(void)
{
	return B11669_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_11669 - input soft stack ready lamp.
 *
 * Always returns 1. Soft pure-data readiness for the 11661-11670
 * input soft id stub continuum; does not open devices or probe udev.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_11669(void)
{
	(void)NULL;
	return b11669_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_11669(void)
    __attribute__((alias("gj_input_soft_ready_u_11669")));
