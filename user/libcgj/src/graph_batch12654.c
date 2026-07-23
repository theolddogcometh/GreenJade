/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12654: controller soft Steam Input ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_input_ok_u_12654(void);
 *     - Returns 0 (soft stub: Steam Input / controller API slot is
 *       soft/unprobed; not a runtime Steamworks or steamclient probe).
 *   uint32_t __gj_steam_input_ok_u_12654  (alias)
 *   __libcgj_batch12654_marker = "libcgj-batch12654"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_steam_input_ok_u_12654 surface only; no
 * multi-def. Distinct from gj_steam_input_ok_u_12454 (batch12454),
 * gj_steam_input_ok_u_12054 (batch12054), gj_steam_input_ok_u_11954
 * (batch11954), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12654_marker[] = "libcgj-batch12654";

/* Soft-stub Steam Input ok value (unprobed). */
#define B12654_STEAM_INPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12654_steam_input_ok(void)
{
	return B12654_STEAM_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_input_ok_u_12654 - soft Steam Input ok stub.
 *
 * Always returns 0 (soft/unprobed Steam Input slot). Does not call libc
 * or probe Steam/Steamworks. No parent wires.
 */
uint32_t
gj_steam_input_ok_u_12654(void)
{
	(void)NULL;
	return b12654_steam_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_input_ok_u_12654(void)
    __attribute__((alias("gj_steam_input_ok_u_12654")));
