/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8963: D-Bus ERROR message type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_error_id_8963(void);
 *     - Returns 3 (soft DBUS_MESSAGE_TYPE_ERROR catalog id).
 *       Pure-data stub; not a live D-Bus connection or message I/O.
 *   uint32_t __gj_dbus_error_id_8963  (alias)
 *   __libcgj_batch8963_marker = "libcgj-batch8963"
 *
 * Exclusive continuum CREATE-ONLY (8961-8970: dbus message type id stubs —
 * dbus_method_call_id, dbus_method_return_id, dbus_error_id,
 * dbus_signal_id, dbus_name_ok_u, dbus_path_ok_u, dbus_iface_ok_u,
 * dbus_member_ok_u, dbus_ready_u, batch_id_8970). Unique
 * gj_dbus_error_id_8963 surface only; no multi-def. Catalog id only;
 * no live D-Bus bus I/O. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8963_marker[] = "libcgj-batch8963";

/* Soft DBUS_MESSAGE_TYPE_ERROR (D-Bus wire message type). */
#define B8963_DBUS_ERROR  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8963_error_id(void)
{
	return B8963_DBUS_ERROR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_error_id_8963 - soft ERROR D-Bus message type id.
 *
 * Always returns 3. Catalog id only; does not open a bus connection or
 * send error messages. No parent wires.
 */
uint32_t
gj_dbus_error_id_8963(void)
{
	(void)NULL;
	return b8963_error_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_error_id_8963(void)
    __attribute__((alias("gj_dbus_error_id_8963")));
