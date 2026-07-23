/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13546: networkd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_13546(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-networkd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_networkd_ok_u_13546  (alias)
 *   __libcgj_batch13546_marker = "libcgj-batch13546"
 *
 * Exclusive continuum CREATE-ONLY (13541-13550: dbus soft id stubs —
 * dbus_ok_u_13541, systemd_ok_u_13542, logind_ok_u_13543,
 * journal_ok_u_13544, resolved_ok_u_13545, networkd_ok_u_13546,
 * timedated_ok_u_13547, hostnamed_ok_u_13548,
 * dbus_soft_ready_u_13549, batch_id_13550→13550). Unique surface only;
 * no multi-def. Distinct from gj_networkd_ok_u_13346 /
 * gj_networkd_ok_u_13146 / gj_networkd_ok_u_12946 / gj_networkd_ok_u_12746.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13546_marker[] = "libcgj-batch13546";

/* Soft networkd-ok lamp: always off (not a real runtime probe). */
#define B13546_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13546_ok(void)
{
	return B13546_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_13546 - networkd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-networkd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkd_ok_u_13546(void)
{
	(void)NULL;
	return b13546_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_13546(void)
    __attribute__((alias("gj_networkd_ok_u_13546")));
