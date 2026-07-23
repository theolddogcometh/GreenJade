/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14357: haptics ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_haptics_ok_u_14357(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       haptics / force-feedback runtime probe for the controller soft
 *       continuum.
 *   uint32_t __gj_haptics_ok_u_14357  (alias)
 *   __libcgj_batch14357_marker = "libcgj-batch14357"
 *
 * Exclusive continuum CREATE-ONLY (14351-14360: controller soft all→0 —
 * controller_ok_u_14351, gamepad_ok_u_14352,
 * steamdeck_controls_ok_u_14353, hid_ok_u_14354, xinput_ok_u_14355,
 * evdev_ok_u_14356, haptics_ok_u_14357, gyro_ok_u_14358,
 * controller_soft_ready_u_14359, batch_id_14360). Unique surface only;
 * no multi-def. Distinct from gj_haptics_ok_u_10557 (batch10557).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14357_marker[] = "libcgj-batch14357";

/* Soft haptics-ok lamp: always off (not a real FF probe). */
#define B14357_HAPTICS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14357_ok(void)
{
	return B14357_HAPTICS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_haptics_ok_u_14357 - haptics ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not drive force-feedback
 * or call libc. No parent wires.
 */
uint32_t
gj_haptics_ok_u_14357(void)
{
	(void)NULL;
	return b14357_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_haptics_ok_u_14357(void)
    __attribute__((alias("gj_haptics_ok_u_14357")));
