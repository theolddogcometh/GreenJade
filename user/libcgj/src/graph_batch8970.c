/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8970: dbus message type id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8970(void);
 *     - Returns the compile-time graph batch number for this TU (8970).
 *   uint32_t __gj_batch_id_8970  (alias)
 *   __libcgj_batch8970_marker = "libcgj-batch8970"
 *
 * Exclusive continuum CREATE-ONLY (8961-8970: dbus message type id stubs —
 * dbus_method_call_id, dbus_method_return_id, dbus_error_id,
 * dbus_signal_id, dbus_name_ok_u, dbus_path_ok_u, dbus_iface_ok_u,
 * dbus_member_ok_u, dbus_ready_u, batch_id_8970). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128. No live D-Bus bus I/O.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8970_marker[] = "libcgj-batch8970";

#define B8970_BATCH_ID  8970u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8970_id(void)
{
	return B8970_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8970 - report this TU's graph batch number.
 *
 * Always returns 8970. Link-time presence tags the exclusive dbus
 * message-type-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_8970(void)
{
	(void)NULL;
	return b8970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8970(void)
    __attribute__((alias("gj_batch_id_8970")));
