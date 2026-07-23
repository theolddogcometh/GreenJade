/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14359: soft controller continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_14359(void);
 *     - Returns 1 (controller soft all→0 continuum ready).
 *       Pure-data product tag that exclusive wave 14351-14360 surfaces
 *       are present for the controller soft path; not a hard pad
 *       runtime probe.
 *   uint32_t __gj_controller_soft_ready_u_14359  (alias)
 *   __libcgj_batch14359_marker = "libcgj-batch14359"
 *
 * Exclusive continuum CREATE-ONLY (14351-14360: controller soft all→0 —
 * controller_ok_u_14351, gamepad_ok_u_14352,
 * steamdeck_controls_ok_u_14353, hid_ok_u_14354, xinput_ok_u_14355,
 * evdev_ok_u_14356, haptics_ok_u_14357, gyro_ok_u_14358,
 * controller_soft_ready_u_14359, batch_id_14360). Unique surface only;
 * no multi-def. Ok units remain 0. Distinct from
 * gj_controller_soft_ready_u_10559 (batch10559). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14359_marker[] = "libcgj-batch14359";

/* Soft continuum-ready lamp for controller soft all→0 wave. */
#define B14359_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14359_soft_ready(void)
{
	return B14359_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_14359 - controller soft continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_14359(void)
{
	(void)NULL;
	return b14359_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_14359(void)
    __attribute__((alias("gj_controller_soft_ready_u_14359")));
