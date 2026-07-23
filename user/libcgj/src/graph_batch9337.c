/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9337: libinput tablet ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_li_tablet_ok_u_9337(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       libinput tablet-tool/pad validity probe.
 *   uint32_t __gj_li_tablet_ok_u_9337  (alias)
 *   __libcgj_batch9337_marker = "libcgj-batch9337"
 *
 * Exclusive continuum CREATE-ONLY (9331-9340: libinput soft id stubs —
 * li_event_pointer_id_9331, li_event_keyboard_id_9332,
 * li_event_touch_id_9333, li_event_gesture_id_9334,
 * li_device_ok_u_9335, li_seat_ok_u_9336, li_tablet_ok_u_9337,
 * li_switch_ok_u_9338, libinput_ready_u_9339, batch_id_9340).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9337_marker[] = "libcgj-batch9337";

/* Soft libinput tablet-ok lamp: always off (not a real tablet probe). */
#define B9337_LI_TABLET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9337_tablet_ok(void)
{
	return B9337_LI_TABLET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_li_tablet_ok_u_9337 - libinput tablet ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open tablet devices
 * or call libinput/libc. No parent wires.
 */
uint32_t
gj_li_tablet_ok_u_9337(void)
{
	(void)NULL;
	return b9337_tablet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_li_tablet_ok_u_9337(void)
    __attribute__((alias("gj_li_tablet_ok_u_9337")));
