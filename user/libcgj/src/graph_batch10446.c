/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10446: networkd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_10446(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-networkd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_networkd_ok_u_10446  (alias)
 *   __libcgj_batch10446_marker = "libcgj-batch10446"
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

const char __libcgj_batch10446_marker[] = "libcgj-batch10446";

/* Soft networkd-ok lamp: always off (not a real runtime probe). */
#define B10446_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10446_ok(void)
{
	return B10446_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_10446 - networkd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-networkd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkd_ok_u_10446(void)
{
	(void)NULL;
	return b10446_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_10446(void)
    __attribute__((alias("gj_networkd_ok_u_10446")));
