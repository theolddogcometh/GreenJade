/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10553: steamdeck controls ok soft unit
 * (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamdeck_controls_ok_u_10553(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Deck controls runtime probe for the controller soft
 *       continuum.
 *   uint32_t __gj_steamdeck_controls_ok_u_10553  (alias)
 *   __libcgj_batch10553_marker = "libcgj-batch10553"
 *
 * Exclusive continuum CREATE-ONLY (10551-10560: controller soft all→0 —
 * controller_ok_u_10551, gamepad_ok_u_10552,
 * steamdeck_controls_ok_u_10553, hid_ok_u_10554, xinput_ok_u_10555,
 * evdev_ok_u_10556, haptics_ok_u_10557, gyro_ok_u_10558,
 * controller_soft_ready_u_10559, batch_id_10560). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10553_marker[] = "libcgj-batch10553";

/* Soft steamdeck-controls-ok lamp: always off (not a real probe). */
#define B10553_STEAMDECK_CONTROLS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10553_ok(void)
{
	return B10553_STEAMDECK_CONTROLS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamdeck_controls_ok_u_10553 - steamdeck controls ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam Deck
 * controls presence or call libc. No parent wires.
 */
uint32_t
gj_steamdeck_controls_ok_u_10553(void)
{
	(void)NULL;
	return b10553_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamdeck_controls_ok_u_10553(void)
    __attribute__((alias("gj_steamdeck_controls_ok_u_10553")));
