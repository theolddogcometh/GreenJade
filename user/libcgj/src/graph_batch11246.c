/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11246: networkd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_11246(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-networkd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_networkd_ok_u_11246  (alias)
 *   __libcgj_batch11246_marker = "libcgj-batch11246"
 *
 * Exclusive continuum CREATE-ONLY (11241-11250: dbus soft id stubs —
 * dbus_ok_u_11241, systemd_ok_u_11242, logind_ok_u_11243,
 * journal_ok_u_11244, resolved_ok_u_11245, networkd_ok_u_11246,
 * timedated_ok_u_11247, hostnamed_ok_u_11248,
 * dbus_soft_ready_u_11249, batch_id_11250). Unique surface only;
 * no multi-def. Distinct from gj_networkd_ok_u_11046 /
 * gj_networkd_ok_u_10846 / gj_networkd_ok_u_10646 /
 * gj_networkd_ok_u_10446 / gj_networkd_ok_u_10146. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11246_marker[] = "libcgj-batch11246";

/* Soft networkd-ok lamp: always off (not a real runtime probe). */
#define B11246_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11246_ok(void)
{
	return B11246_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_11246 - networkd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-networkd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkd_ok_u_11246(void)
{
	(void)NULL;
	return b11246_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_11246(void)
    __attribute__((alias("gj_networkd_ok_u_11246")));
