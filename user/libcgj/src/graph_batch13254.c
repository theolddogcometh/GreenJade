/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13254: controller soft Steam Input ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_input_ok_u_13254(void);
 *     - Returns 0 (soft stub: Steam Input / controller API slot is
 *       soft/unprobed; not a runtime Steamworks or steamclient probe).
 *   uint32_t __gj_steam_input_ok_u_13254  (alias)
 *   __libcgj_batch13254_marker = "libcgj-batch13254"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_steam_input_ok_u_13254 surface only; no
 * multi-def. Distinct from gj_steam_input_ok_u_13054 (batch13054),
 * gj_steam_input_ok_u_12854 (batch12854), gj_steam_input_ok_u_12654
 * (batch12654), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13254_marker[] = "libcgj-batch13254";

/* Soft-stub Steam Input ok value (unprobed). */
#define B13254_STEAM_INPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13254_steam_input_ok(void)
{
	return B13254_STEAM_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_input_ok_u_13254 - soft Steam Input ok stub.
 *
 * Always returns 0 (soft/unprobed Steam Input slot). Does not call libc
 * or probe Steam/Steamworks. No parent wires.
 */
uint32_t
gj_steam_input_ok_u_13254(void)
{
	(void)NULL;
	return b13254_steam_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_input_ok_u_13254(void)
    __attribute__((alias("gj_steam_input_ok_u_13254")));
