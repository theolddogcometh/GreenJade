/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11954: controller soft Steam Input ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_input_ok_u_11954(void);
 *     - Returns 0 (soft stub: Steam Input / controller API slot is
 *       soft/unprobed; not a runtime Steamworks or steamclient probe).
 *   uint32_t __gj_steam_input_ok_u_11954  (alias)
 *   __libcgj_batch11954_marker = "libcgj-batch11954"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_steam_input_ok_u_11954 surface only; no
 * multi-def. Distinct from gj_steam_input_ok_u_11754 (batch11754),
 * gj_steam_input_ok_u_11554 (batch11554), and sibling controller soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11954_marker[] = "libcgj-batch11954";

/* Soft-stub Steam Input ok value (unprobed). */
#define B11954_STEAM_INPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11954_steam_input_ok(void)
{
	return B11954_STEAM_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_input_ok_u_11954 - soft Steam Input ok stub.
 *
 * Always returns 0 (soft/unprobed Steam Input slot). Does not call libc
 * or probe Steam/Steamworks. No parent wires.
 */
uint32_t
gj_steam_input_ok_u_11954(void)
{
	(void)NULL;
	return b11954_steam_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_input_ok_u_11954(void)
    __attribute__((alias("gj_steam_input_ok_u_11954")));
