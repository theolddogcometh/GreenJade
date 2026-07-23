/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14059: udev/libinput soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_libinput_ready_u_14059(void);
 *     - Returns 1 (soft lamp only). Indicates the udev/libinput soft-stub
 *       surfaces in this continuum are present; not device/seat readiness.
 *   uint32_t __gj_udev_libinput_ready_u_14059  (alias)
 *   __libcgj_batch14059_marker = "libcgj-batch14059"
 *
 * Exclusive continuum CREATE-ONLY (14051-14060: udev/libinput soft —
 * udev_li_action_add_id_14051, udev_li_action_remove_id_14052,
 * udev_li_action_change_id_14053, udev_li_subsystem_input_id_14054,
 * li_udev_event_pointer_id_14055, li_udev_event_keyboard_id_14056,
 * udev_li_device_ok_u_14057, udev_li_seat_ok_u_14058,
 * udev_libinput_ready_u_14059, batch_id_14060).
 * Unique surface only; no multi-def. Device/seat ok units remain 0.
 * No parent wires. No __int128. No udev/libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14059_marker[] = "libcgj-batch14059";

/* Soft continuum lamp: udev/libinput soft-stub surfaces present. */
#define B14059_UDEV_LIBINPUT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14059_udev_libinput_ready(void)
{
	return B14059_UDEV_LIBINPUT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_libinput_ready_u_14059 - udev/libinput soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14051-14060 surfaces are present. Does not claim device/seat ready
 * and does not call libudev/libinput/libc. No parent wires.
 */
uint32_t
gj_udev_libinput_ready_u_14059(void)
{
	(void)NULL;
	return b14059_udev_libinput_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_libinput_ready_u_14059(void)
    __attribute__((alias("gj_udev_libinput_ready_u_14059")));
