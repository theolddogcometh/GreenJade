/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8965: soft D-Bus well-known name ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_name_ok_u_8965(void);
 *     - Returns 1 (D-Bus bus-name soft-id continuum ok). Pure-data
 *       product tag; does not parse or claim bus names.
 *   uint32_t __gj_dbus_name_ok_u_8965  (alias)
 *   __libcgj_batch8965_marker = "libcgj-batch8965"
 *
 * Exclusive continuum CREATE-ONLY (8961-8970: dbus message type id stubs —
 * dbus_method_call_id, dbus_method_return_id, dbus_error_id,
 * dbus_signal_id, dbus_name_ok_u, dbus_path_ok_u, dbus_iface_ok_u,
 * dbus_member_ok_u, dbus_ready_u, batch_id_8970). Unique
 * gj_dbus_name_ok_u_8965 surface only; no multi-def. No parent wires.
 * No __int128. No live D-Bus bus I/O.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8965_marker[] = "libcgj-batch8965";

#define B8965_NAME_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8965_name_ok(void)
{
	return B8965_NAME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_name_ok_u_8965 - D-Bus bus-name soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse bus
 * names or call libc. No parent wires.
 */
uint32_t
gj_dbus_name_ok_u_8965(void)
{
	(void)NULL;
	return b8965_name_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_name_ok_u_8965(void)
    __attribute__((alias("gj_dbus_name_ok_u_8965")));
