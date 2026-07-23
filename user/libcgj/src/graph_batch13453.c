/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13453: Steam Deck controls ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamdeck_controls_ok_u_13453(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck controls probe for the controller/input continuum.
 *   uint32_t __gj_steamdeck_controls_ok_u_13453  (alias)
 *   __libcgj_batch13453_marker = "libcgj-batch13453"
 *
 * Exclusive continuum CREATE-ONLY (13451-13460: controller soft all→0
 * stubs — controller_ok_u_13451, gamepad_ok_u_13452,
 * steamdeck_controls_ok_u_13453, hid_ok_u_13454, xinput_ok_u_13455,
 * evdev_ok_u_13456, haptics_ok_u_13457, gyro_ok_u_13458,
 * controller_soft_ready_u_13459, batch_id_13460). Unique surface only;
 * no multi-def. No parent wires. No __int128. No Steam Deck
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13453_marker[] = "libcgj-batch13453";

/* Soft Steam-Deck-controls-ok lamp: always off (not a real probe). */
#define B13453_STEAMDECK_CONTROLS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13453_steamdeck_controls_ok(void)
{
	return B13453_STEAMDECK_CONTROLS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamdeck_controls_ok_u_13453 - Steam Deck controls ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_steamdeck_controls_ok_u_13453(void)
{
	(void)NULL;
	return b13453_steamdeck_controls_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamdeck_controls_ok_u_13453(void)
    __attribute__((alias("gj_steamdeck_controls_ok_u_13453")));
