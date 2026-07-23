/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14060: udev/libinput soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14060(void);
 *     - Returns the compile-time graph batch number for this TU (14060).
 *   uint32_t __gj_batch_id_14060  (alias)
 *   __libcgj_batch14060_marker = "libcgj-batch14060"
 *
 * Exclusive continuum CREATE-ONLY (14051-14060: udev/libinput soft —
 * udev_li_action_add_id_14051, udev_li_action_remove_id_14052,
 * udev_li_action_change_id_14053, udev_li_subsystem_input_id_14054,
 * li_udev_event_pointer_id_14055, li_udev_event_keyboard_id_14056,
 * udev_li_device_ok_u_14057, udev_li_seat_ok_u_14058,
 * udev_libinput_ready_u_14059, batch_id_14060).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No udev/libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14060_marker[] = "libcgj-batch14060";

#define B14060_BATCH_ID  14060u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14060_id(void)
{
	return B14060_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14060 - report this TU's graph batch number.
 *
 * Always returns 14060. Link-time presence tags the exclusive
 * udev/libinput soft continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_14060(void)
{
	(void)NULL;
	return b14060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14060(void)
    __attribute__((alias("gj_batch_id_14060")));
