/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13941: dbus ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_ok_u_13941(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       D-Bus runtime probe for the dbus soft continuum.
 *   uint32_t __gj_dbus_ok_u_13941  (alias)
 *   __libcgj_batch13941_marker = "libcgj-batch13941"
 *
 * Exclusive continuum CREATE-ONLY (13941-13950: dbus soft id stubs —
 * dbus_ok_u_13941, systemd_ok_u_13942, logind_ok_u_13943,
 * journal_ok_u_13944, resolved_ok_u_13945, networkd_ok_u_13946,
 * timedated_ok_u_13947, hostnamed_ok_u_13948,
 * dbus_soft_ready_u_13949, batch_id_13950→13950). Unique surface
 * only; no multi-def. Distinct from gj_dbus_ok_u_11841 /
 * gj_dbus_ok_u_11641 / gj_dbus_ok_u_11441 / gj_dbus_ok_u_11241 /
 * gj_dbus_ok_u_11041 / gj_dbus_ok_u_10841 / gj_dbus_ok_u_10641 /
 * gj_dbus_ok_u_10441 / gj_dbus_ok_u_10141. No parent wires.
 * No __int128. No D-Bus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13941_marker[] = "libcgj-batch13941";

/* Soft dbus-ok lamp: always off (not a real runtime probe). */
#define B13941_DBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13941_ok(void)
{
	return B13941_DBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_ok_u_13941 - dbus ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe D-Bus
 * presence or call libc. No parent wires.
 */
uint32_t
gj_dbus_ok_u_13941(void)
{
	(void)NULL;
	return b13941_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_ok_u_13941(void)
    __attribute__((alias("gj_dbus_ok_u_13941")));
