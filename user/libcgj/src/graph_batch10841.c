/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10841: dbus ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_ok_u_10841(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       D-Bus runtime probe for the dbus soft continuum.
 *   uint32_t __gj_dbus_ok_u_10841  (alias)
 *   __libcgj_batch10841_marker = "libcgj-batch10841"
 *
 * Exclusive continuum CREATE-ONLY (10841-10850: dbus soft id stubs —
 * dbus_ok_u_10841, systemd_ok_u_10842, logind_ok_u_10843,
 * journal_ok_u_10844, resolved_ok_u_10845, networkd_ok_u_10846,
 * timedated_ok_u_10847, hostnamed_ok_u_10848,
 * dbus_soft_ready_u_10849, batch_id_10850). Unique surface only;
 * no multi-def. Distinct from gj_dbus_ok_u_10641 / gj_dbus_ok_u_10441 /
 * gj_dbus_ok_u_10141. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10841_marker[] = "libcgj-batch10841";

/* Soft dbus-ok lamp: always off (not a real runtime probe). */
#define B10841_DBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10841_ok(void)
{
	return B10841_DBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_ok_u_10841 - dbus ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe D-Bus bus
 * presence or call libc. No parent wires.
 */
uint32_t
gj_dbus_ok_u_10841(void)
{
	(void)NULL;
	return b10841_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_ok_u_10841(void)
    __attribute__((alias("gj_dbus_ok_u_10841")));
