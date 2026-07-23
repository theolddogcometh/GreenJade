/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8964: D-Bus SIGNAL message type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_signal_id_8964(void);
 *     - Returns 4 (soft DBUS_MESSAGE_TYPE_SIGNAL catalog id).
 *       Pure-data stub; not a live D-Bus connection or message I/O.
 *   uint32_t __gj_dbus_signal_id_8964  (alias)
 *   __libcgj_batch8964_marker = "libcgj-batch8964"
 *
 * Exclusive continuum CREATE-ONLY (8961-8970: dbus message type id stubs —
 * dbus_method_call_id, dbus_method_return_id, dbus_error_id,
 * dbus_signal_id, dbus_name_ok_u, dbus_path_ok_u, dbus_iface_ok_u,
 * dbus_member_ok_u, dbus_ready_u, batch_id_8970). Unique
 * gj_dbus_signal_id_8964 surface only; no multi-def. Catalog id only;
 * no live D-Bus bus I/O. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8964_marker[] = "libcgj-batch8964";

/* Soft DBUS_MESSAGE_TYPE_SIGNAL (D-Bus wire message type). */
#define B8964_DBUS_SIGNAL  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8964_signal_id(void)
{
	return B8964_DBUS_SIGNAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_signal_id_8964 - soft SIGNAL D-Bus message type id.
 *
 * Always returns 4. Catalog id only; does not open a bus connection or
 * emit signals. No parent wires.
 */
uint32_t
gj_dbus_signal_id_8964(void)
{
	(void)NULL;
	return b8964_signal_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_signal_id_8964(void)
    __attribute__((alias("gj_dbus_signal_id_8964")));
