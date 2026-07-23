/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14056: soft libinput/udev keyboard event id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_li_udev_event_keyboard_id_14056(void);
 *     - Returns 300 (keyboard event-class soft catalog value). Pure-data
 *       event-type id stub; does not open devices or call libinput/udev.
 *   uint32_t __gj_li_udev_event_keyboard_id_14056  (alias)
 *   __libcgj_batch14056_marker = "libcgj-batch14056"
 *
 * Exclusive continuum CREATE-ONLY (14051-14060: udev/libinput soft —
 * udev_li_action_add_id_14051, udev_li_action_remove_id_14052,
 * udev_li_action_change_id_14053, udev_li_subsystem_input_id_14054,
 * li_udev_event_pointer_id_14055, li_udev_event_keyboard_id_14056,
 * udev_li_device_ok_u_14057, udev_li_seat_ok_u_14058,
 * udev_libinput_ready_u_14059, batch_id_14060).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No udev/libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14056_marker[] = "libcgj-batch14056";

/* Soft keyboard event-class catalog id (300). */
#define B14056_LI_UDEV_EVENT_KEYBOARD  300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14056_event_keyboard_id(void)
{
	return B14056_LI_UDEV_EVENT_KEYBOARD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_li_udev_event_keyboard_id_14056 - soft keyboard event catalog id.
 *
 * Always returns 300. Soft continuum constant; does not open input
 * devices or call libinput/libudev/libc. No parent wires.
 */
uint32_t
gj_li_udev_event_keyboard_id_14056(void)
{
	(void)NULL;
	return b14056_event_keyboard_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_li_udev_event_keyboard_id_14056(void)
    __attribute__((alias("gj_li_udev_event_keyboard_id_14056")));
