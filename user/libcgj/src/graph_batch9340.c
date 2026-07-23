/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9340: libinput soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9340(void);
 *     - Returns the compile-time graph batch number for this TU (9340).
 *   uint32_t __gj_batch_id_9340  (alias)
 *   __libcgj_batch9340_marker = "libcgj-batch9340"
 *
 * Exclusive continuum CREATE-ONLY (9331-9340: libinput soft id stubs —
 * li_event_pointer_id_9331, li_event_keyboard_id_9332,
 * li_event_touch_id_9333, li_event_gesture_id_9334,
 * li_device_ok_u_9335, li_seat_ok_u_9336, li_tablet_ok_u_9337,
 * li_switch_ok_u_9338, libinput_ready_u_9339, batch_id_9340).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9340_marker[] = "libcgj-batch9340";

#define B9340_BATCH_ID  9340u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9340_id(void)
{
	return B9340_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9340 - report this TU's graph batch number.
 *
 * Always returns 9340. Link-time presence tags the exclusive
 * libinput soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9340(void)
{
	(void)NULL;
	return b9340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9340(void)
    __attribute__((alias("gj_batch_id_9340")));
