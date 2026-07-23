/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12141: dbus soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_ok_u_12141(void);
 *     - Returns 0 (soft stub: dbus presence is soft/unprobed; not a
 *       runtime bus or sd-bus probe).
 *   uint32_t __gj_dbus_ok_u_12141  (alias)
 *   __libcgj_batch12141_marker = "libcgj-batch12141"
 *
 * Exclusive continuum CREATE-ONLY (12141-12150: dbus/systemd soft id
 * stubs). Unique gj_dbus_ok_u_12141 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12141_marker[] = "libcgj-batch12141";

/* Soft-stub dbus ok value (unprobed / not present). */
#define B12141_DBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12141_dbus_ok(void)
{
	return B12141_DBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_ok_u_12141 - soft dbus presence stub.
 *
 * Always returns 0 (soft/unprobed dbus slot). Does not call libc or
 * probe the system bus. No parent wires.
 */
uint32_t
gj_dbus_ok_u_12141(void)
{
	(void)NULL;
	return b12141_dbus_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_ok_u_12141(void)
    __attribute__((alias("gj_dbus_ok_u_12141")));
