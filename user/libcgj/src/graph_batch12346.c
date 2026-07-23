/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12346: networkd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_networkd_ok_u_12346(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-networkd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_networkd_ok_u_12346  (alias)
 *   __libcgj_batch12346_marker = "libcgj-batch12346"
 *
 * Exclusive continuum CREATE-ONLY (12341-12350: dbus soft id stubs —
 * dbus_ok_u_12341, systemd_ok_u_12342, logind_ok_u_12343,
 * journal_ok_u_12344, resolved_ok_u_12345, networkd_ok_u_12346,
 * timedated_ok_u_12347, hostnamed_ok_u_12348,
 * dbus_soft_ready_u_12349, batch_id_12350). Unique surface only;
 * no multi-def. Distinct from gj_networkd_ok_u_12146 / gj_networkd_ok_u_11846
 * / gj_networkd_ok_u_11646.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12346_marker[] = "libcgj-batch12346";

/* Soft networkd-ok lamp: always off (not a real runtime probe). */
#define B12346_NETWORKD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12346_ok(void)
{
	return B12346_NETWORKD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_networkd_ok_u_12346 - networkd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe network1 link
 * presence or call libc. No parent wires.
 */
uint32_t
gj_networkd_ok_u_12346(void)
{
	(void)NULL;
	return b12346_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_networkd_ok_u_12346(void)
    __attribute__((alias("gj_networkd_ok_u_12346")));
