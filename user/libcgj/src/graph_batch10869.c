/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10869: input soft stack ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_10869(void);
 *     - Returns 1 (input soft id stub surface considered ready for this
 *       continuum wave). Soft compile-time product tag; not a live
 *       input subsystem readiness probe.
 *   uint32_t __gj_input_soft_ready_u_10869  (alias)
 *   __libcgj_batch10869_marker = "libcgj-batch10869"
 *
 * Exclusive continuum CREATE-ONLY (10861-10870: input soft —
 * udev_ok_u_10861, libinput_ok_u_10862, evdev_ok_u_10863,
 * hidraw_ok_u_10864, uinput_ok_u_10865, hotplug_ok_u_10866,
 * device_db_ok_u_10867, hwdb_ok_u_10868, input_soft_ready_u_10869,
 * batch_id_10870). Unique surface only; no multi-def. Distinct from
 * prior input soft ready lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10869_marker[] = "libcgj-batch10869";

/* Soft lamp: input soft stack ready for this continuum. */
#define B10869_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10869_input_soft_ready(void)
{
	return B10869_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_10869 - input soft stack ready lamp.
 *
 * Always returns 1. Soft pure-data readiness for the 10861-10870
 * input soft id stub continuum; does not open devices or probe udev.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_10869(void)
{
	(void)NULL;
	return b10869_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_10869(void)
    __attribute__((alias("gj_input_soft_ready_u_10869")));
