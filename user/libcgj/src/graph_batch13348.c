/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13348: hostnamed ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_13348(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-hostnamed runtime probe for the dbus soft continuum.
 *   uint32_t __gj_hostnamed_ok_u_13348  (alias)
 *   __libcgj_batch13348_marker = "libcgj-batch13348"
 *
 * Exclusive continuum CREATE-ONLY (13341-13350: dbus soft id stubs —
 * dbus_ok_u_13341, systemd_ok_u_13342, logind_ok_u_13343,
 * journal_ok_u_13344, resolved_ok_u_13345, networkd_ok_u_13346,
 * timedated_ok_u_13347, hostnamed_ok_u_13348,
 * dbus_soft_ready_u_13349, batch_id_13350→13350). Unique surface only;
 * no multi-def. Distinct from gj_hostnamed_ok_u_13148 /
 * gj_hostnamed_ok_u_12948 / gj_hostnamed_ok_u_12748. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13348_marker[] = "libcgj-batch13348";

/* Soft hostnamed-ok lamp: always off (not a real runtime probe). */
#define B13348_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13348_ok(void)
{
	return B13348_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_13348 - hostnamed ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-hostnamed
 * presence or call libc. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_13348(void)
{
	(void)NULL;
	return b13348_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_13348(void)
    __attribute__((alias("gj_hostnamed_ok_u_13348")));
