/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14238: hostnamed ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_14238(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-hostnamed runtime probe for the dbus soft continuum.
 *   uint32_t __gj_hostnamed_ok_u_14238  (alias)
 *   __libcgj_batch14238_marker = "libcgj-batch14238"
 *
 * Exclusive continuum CREATE-ONLY (14231-14240: dbus soft id stubs —
 * dbus_ok_u_14231, systemd_ok_u_14232, logind_ok_u_14233,
 * journal_ok_u_14234, resolved_ok_u_14235, networkd_ok_u_14236,
 * timedated_ok_u_14237, hostnamed_ok_u_14238,
 * dbus_soft_ready_u_14239, batch_id_14240). Unique surface only;
 * no multi-def. Distinct from gj_hostnamed_ok_u_13948 /
 * gj_hostnamed_ok_u_13748 / gj_hostnamed_ok_u_13548 /
 * gj_hostnamed_ok_u_13348 / gj_hostnamed_ok_u_13148 /
 * gj_hostnamed_ok_u_12948. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14238_marker[] = "libcgj-batch14238";

/* Soft hostnamed-ok lamp: always off (not a real runtime probe). */
#define B14238_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14238_ok(void)
{
	return B14238_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_14238 - hostnamed ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-hostnamed
 * presence or call libc. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_14238(void)
{
	(void)NULL;
	return b14238_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_14238(void)
    __attribute__((alias("gj_hostnamed_ok_u_14238")));
