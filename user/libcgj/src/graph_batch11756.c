/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11756: controller soft evdev-pad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_pad_ok_u_11756(void);
 *     - Returns 0 (soft stub: evdev gamepad/pad slot is soft/unprobed;
 *       not a runtime /dev/input/event* or libevdev probe).
 *   uint32_t __gj_evdev_pad_ok_u_11756  (alias)
 *   __libcgj_batch11756_marker = "libcgj-batch11756"
 *
 * Exclusive continuum CREATE-ONLY (11751-11760: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11760). Unique
 * gj_evdev_pad_ok_u_11756 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11756_marker[] = "libcgj-batch11756";

/* Soft-stub evdev pad ok value (unprobed). */
#define B11756_EVDEV_PAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11756_evdev_pad_ok(void)
{
	return B11756_EVDEV_PAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_pad_ok_u_11756 - soft evdev pad ok stub.
 *
 * Always returns 0 (soft/unprobed evdev pad slot). Does not call libc
 * or probe /dev/input/event*. No parent wires.
 */
uint32_t
gj_evdev_pad_ok_u_11756(void)
{
	(void)NULL;
	return b11756_evdev_pad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_pad_ok_u_11756(void)
    __attribute__((alias("gj_evdev_pad_ok_u_11756")));
