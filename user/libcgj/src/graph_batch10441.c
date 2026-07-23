/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10441: dbus ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_ok_u_10441(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       D-Bus runtime probe for the dbus soft continuum.
 *   uint32_t __gj_dbus_ok_u_10441  (alias)
 *   __libcgj_batch10441_marker = "libcgj-batch10441"
 *
 * Exclusive continuum CREATE-ONLY (10441-10450: dbus soft id stubs —
 * dbus_ok_u_10441, systemd_ok_u_10442, logind_ok_u_10443,
 * journal_ok_u_10444, resolved_ok_u_10445, networkd_ok_u_10446,
 * timedated_ok_u_10447, hostnamed_ok_u_10448,
 * dbus_soft_ready_u_10449, batch_id_10450). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10441_marker[] = "libcgj-batch10441";

/* Soft dbus-ok lamp: always off (not a real runtime probe). */
#define B10441_DBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10441_ok(void)
{
	return B10441_DBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_ok_u_10441 - dbus ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe D-Bus bus
 * presence or call libc. No parent wires.
 */
uint32_t
gj_dbus_ok_u_10441(void)
{
	(void)NULL;
	return b10441_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_ok_u_10441(void)
    __attribute__((alias("gj_dbus_ok_u_10441")));
