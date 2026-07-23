/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11753: controller soft joystick ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joystick_ok_u_11753(void);
 *     - Returns 0 (soft stub: joystick slot is soft/unprobed; not a
 *       runtime joystick enumeration or /dev/input/js* probe).
 *   uint32_t __gj_joystick_ok_u_11753  (alias)
 *   __libcgj_batch11753_marker = "libcgj-batch11753"
 *
 * Exclusive continuum CREATE-ONLY (11751-11760: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11760). Unique
 * gj_joystick_ok_u_11753 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11753_marker[] = "libcgj-batch11753";

/* Soft-stub joystick ok value (unprobed). */
#define B11753_JOYSTICK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11753_joystick_ok(void)
{
	return B11753_JOYSTICK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joystick_ok_u_11753 - soft joystick ok stub.
 *
 * Always returns 0 (soft/unprobed joystick slot). Does not call libc or
 * probe joystick devices. No parent wires.
 */
uint32_t
gj_joystick_ok_u_11753(void)
{
	(void)NULL;
	return b11753_joystick_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joystick_ok_u_11753(void)
    __attribute__((alias("gj_joystick_ok_u_11753")));
