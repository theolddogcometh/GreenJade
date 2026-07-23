/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14054: soft udev/libinput input subsystem id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_li_subsystem_input_id_14054(void);
 *     - Returns 100 (input subsystem soft catalog id). Pure-data
 *       subsystem id stub; does not enumerate or call libudev/libinput.
 *   uint32_t __gj_udev_li_subsystem_input_id_14054  (alias)
 *   __libcgj_batch14054_marker = "libcgj-batch14054"
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

const char __libcgj_batch14054_marker[] = "libcgj-batch14054";

/* Soft udev/libinput "input" subsystem catalog id. */
#define B14054_UDEV_LI_SUBSYSTEM_INPUT  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14054_subsystem_input_id(void)
{
	return B14054_UDEV_LI_SUBSYSTEM_INPUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_li_subsystem_input_id_14054 - soft input subsystem catalog id.
 *
 * Always returns 100. Soft continuum constant; does not enumerate
 * subsystems or call libudev/libinput/libc. No parent wires.
 */
uint32_t
gj_udev_li_subsystem_input_id_14054(void)
{
	(void)NULL;
	return b14054_subsystem_input_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_li_subsystem_input_id_14054(void)
    __attribute__((alias("gj_udev_li_subsystem_input_id_14054")));
