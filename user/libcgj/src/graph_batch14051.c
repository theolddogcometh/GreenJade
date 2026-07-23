/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14051: soft udev/libinput action-add id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_li_action_add_id_14051(void);
 *     - Returns 1 (udev "add" action soft catalog id). Pure-data
 *       action id stub; does not watch devices or call libudev/libinput.
 *   uint32_t __gj_udev_li_action_add_id_14051  (alias)
 *   __libcgj_batch14051_marker = "libcgj-batch14051"
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

const char __libcgj_batch14051_marker[] = "libcgj-batch14051";

/* Soft udev "add" action catalog id. */
#define B14051_UDEV_LI_ACTION_ADD  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14051_action_add_id(void)
{
	return B14051_UDEV_LI_ACTION_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_li_action_add_id_14051 - soft udev/libinput "add" action id.
 *
 * Always returns 1. Soft continuum constant; does not watch devices or
 * call libudev/libinput/libc. No parent wires.
 */
uint32_t
gj_udev_li_action_add_id_14051(void)
{
	(void)NULL;
	return b14051_action_add_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_li_action_add_id_14051(void)
    __attribute__((alias("gj_udev_li_action_add_id_14051")));
