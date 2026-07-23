/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8961: D-Bus METHOD_CALL message type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_method_call_id_8961(void);
 *     - Returns 1 (soft DBUS_MESSAGE_TYPE_METHOD_CALL catalog id).
 *       Pure-data stub; not a live D-Bus connection or message I/O.
 *   uint32_t __gj_dbus_method_call_id_8961  (alias)
 *   __libcgj_batch8961_marker = "libcgj-batch8961"
 *
 * Exclusive continuum CREATE-ONLY (8961-8970: dbus message type id stubs —
 * dbus_method_call_id, dbus_method_return_id, dbus_error_id,
 * dbus_signal_id, dbus_name_ok_u, dbus_path_ok_u, dbus_iface_ok_u,
 * dbus_member_ok_u, dbus_ready_u, batch_id_8970). Unique
 * gj_dbus_method_call_id_8961 surface only; no multi-def. Catalog id
 * only; no live D-Bus bus I/O. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8961_marker[] = "libcgj-batch8961";

/* Soft DBUS_MESSAGE_TYPE_METHOD_CALL (D-Bus wire message type). */
#define B8961_DBUS_METHOD_CALL  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8961_method_call_id(void)
{
	return B8961_DBUS_METHOD_CALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_method_call_id_8961 - soft METHOD_CALL D-Bus message type id.
 *
 * Always returns 1. Catalog id only; does not open a bus connection or
 * send method_call messages. No parent wires.
 */
uint32_t
gj_dbus_method_call_id_8961(void)
{
	(void)NULL;
	return b8961_method_call_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_method_call_id_8961(void)
    __attribute__((alias("gj_dbus_method_call_id_8961")));
