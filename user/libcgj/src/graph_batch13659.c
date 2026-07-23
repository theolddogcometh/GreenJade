/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13659: controller soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_13659(void);
 *     - Returns 1 (soft lamp only). Indicates the controller soft-id
 *       stub surfaces in this continuum are present; not controller,
 *       gamepad, steamdeck_controls, hid, xinput, evdev, haptics, or
 *       gyro ok.
 *   uint32_t __gj_controller_soft_ready_u_13659  (alias)
 *   __libcgj_batch13659_marker = "libcgj-batch13659"
 *
 * Exclusive continuum CREATE-ONLY (13651-13660: controller soft id
 * stubs — controller_ok_u_13651, gamepad_ok_u_13652,
 * steamdeck_controls_ok_u_13653, hid_ok_u_13654, xinput_ok_u_13655,
 * evdev_ok_u_13656, haptics_ok_u_13657, gyro_ok_u_13658,
 * controller_soft_ready_u_13659, batch_id_13660→13660).
 * Unique surface only; no multi-def. controller/gamepad/
 * steamdeck_controls/hid/xinput/evdev/haptics/gyro ok units remain 0.
 * Distinct from gj_controller_soft_ready_u_13459 (batch13459),
 * gj_controller_soft_ready_u_13259 (batch13259),
 * gj_controller_soft_ready_u_13059 (batch13059),
 * gj_controller_soft_ready_u_12859 (batch12859) and
 * gj_controller_soft_ready_u_12659 (batch12659). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13659_marker[] = "libcgj-batch13659";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B13659_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13659_soft_ready(void)
{
	return B13659_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_13659 - controller soft continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 13651-13660 surfaces are present. Does not claim controller/gamepad/
 * steamdeck_controls/hid/xinput/evdev/haptics/gyro ok and does not
 * call libc. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_13659(void)
{
	(void)NULL;
	return b13659_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_13659(void)
    __attribute__((alias("gj_controller_soft_ready_u_13659")));
