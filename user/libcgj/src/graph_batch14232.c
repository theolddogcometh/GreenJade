/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14232: systemd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_systemd_ok_u_14232(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_systemd_ok_u_14232  (alias)
 *   __libcgj_batch14232_marker = "libcgj-batch14232"
 *
 * Exclusive continuum CREATE-ONLY (14231-14240: dbus soft id stubs —
 * dbus_ok_u_14231, systemd_ok_u_14232, logind_ok_u_14233,
 * journal_ok_u_14234, resolved_ok_u_14235, networkd_ok_u_14236,
 * timedated_ok_u_14237, hostnamed_ok_u_14238,
 * dbus_soft_ready_u_14239, batch_id_14240). Unique surface only;
 * no multi-def. Distinct from gj_systemd_ok_u_13942 /
 * gj_systemd_ok_u_13742 / gj_systemd_ok_u_13542 /
 * gj_systemd_ok_u_13342 / gj_systemd_ok_u_13142 /
 * gj_systemd_ok_u_12942. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14232_marker[] = "libcgj-batch14232";

/* Soft systemd-ok lamp: always off (not a real runtime probe). */
#define B14232_SYSTEMD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14232_ok(void)
{
	return B14232_SYSTEMD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_systemd_ok_u_14232 - systemd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_systemd_ok_u_14232(void)
{
	(void)NULL;
	return b14232_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_systemd_ok_u_14232(void)
    __attribute__((alias("gj_systemd_ok_u_14232")));
