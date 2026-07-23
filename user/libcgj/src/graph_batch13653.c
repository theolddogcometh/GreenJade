/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13653: Steam Deck controls ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamdeck_controls_ok_u_13653(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck controls probe for the controller soft continuum.
 *   uint32_t __gj_steamdeck_controls_ok_u_13653  (alias)
 *   __libcgj_batch13653_marker = "libcgj-batch13653"
 *
 * Exclusive continuum CREATE-ONLY (13651-13660: controller soft id
 * stubs — controller_ok_u_13651, gamepad_ok_u_13652,
 * steamdeck_controls_ok_u_13653, hid_ok_u_13654, xinput_ok_u_13655,
 * evdev_ok_u_13656, haptics_ok_u_13657, gyro_ok_u_13658,
 * controller_soft_ready_u_13659, batch_id_13660→13660).
 * Unique surface only; no multi-def. Distinct from
 * gj_steamdeck_controls_ok_u_13453 (batch13453). No parent wires. No
 * __int128. No Steam Deck implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13653_marker[] = "libcgj-batch13653";

/* Soft Steam-Deck-controls-ok lamp: always off (not a real probe). */
#define B13653_STEAMDECK_CONTROLS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13653_steamdeck_controls_ok(void)
{
	return B13653_STEAMDECK_CONTROLS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamdeck_controls_ok_u_13653 - Steam Deck controls ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_steamdeck_controls_ok_u_13653(void)
{
	(void)NULL;
	return b13653_steamdeck_controls_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamdeck_controls_ok_u_13653(void)
    __attribute__((alias("gj_steamdeck_controls_ok_u_13653")));
