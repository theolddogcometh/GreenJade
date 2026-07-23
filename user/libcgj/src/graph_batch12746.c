/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12746: networkd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_12746(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-networkd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_networkd_ok_u_12746  (alias)
 *   __libcgj_batch12746_marker = "libcgj-batch12746"
 *
 * Exclusive continuum CREATE-ONLY (12741-12750: dbus soft id stubs —
 * dbus_ok_u_12741, systemd_ok_u_12742, logind_ok_u_12743,
 * journal_ok_u_12744, resolved_ok_u_12745, networkd_ok_u_12746,
 * timedated_ok_u_12747, hostnamed_ok_u_12748,
 * dbus_soft_ready_u_12749, batch_id_12750). Unique surface only;
 * no multi-def. Distinct from gj_networkd_ok_u_10646 /
 * gj_networkd_ok_u_10446 / gj_networkd_ok_u_10146. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12746_marker[] = "libcgj-batch12746";

/* Soft networkd-ok lamp: always off (not a real runtime probe). */
#define B12746_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12746_ok(void)
{
	return B12746_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_12746 - networkd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe network1 link
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkd_ok_u_12746(void)
{
	(void)NULL;
	return b12746_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_12746(void)
    __attribute__((alias("gj_networkd_ok_u_12746")));
