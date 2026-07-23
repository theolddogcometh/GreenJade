/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10669: input soft stack ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_10669(void);
 *     - Returns 1 (input soft id stub surface considered ready for this
 *       continuum wave). Soft compile-time product tag; not a live
 *       input subsystem readiness probe.
 *   uint32_t __gj_input_soft_ready_u_10669  (alias)
 *   __libcgj_batch10669_marker = "libcgj-batch10669"
 *
 * Exclusive continuum CREATE-ONLY (10661-10670: input soft —
 * udev_ok_u_10661, libinput_ok_u_10662, evdev_ok_u_10663,
 * hidraw_ok_u_10664, uinput_ok_u_10665, hotplug_ok_u_10666,
 * device_db_ok_u_10667, hwdb_ok_u_10668, input_soft_ready_u_10669,
 * batch_id_10670). Unique surface only; no multi-def. Distinct from
 * prior input soft ready lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10669_marker[] = "libcgj-batch10669";

/* Soft lamp: input soft stack ready for this continuum. */
#define B10669_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10669_input_soft_ready(void)
{
	return B10669_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_10669 - input soft stack ready lamp.
 *
 * Always returns 1. Soft pure-data readiness for the 10661-10670
 * input soft id stub continuum; does not open devices or probe udev.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_10669(void)
{
	(void)NULL;
	return b10669_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_10669(void)
    __attribute__((alias("gj_input_soft_ready_u_10669")));
