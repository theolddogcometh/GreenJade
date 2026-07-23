/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13748: hostnamed ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_13748(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-hostnamed runtime probe for the dbus soft continuum.
 *   uint32_t __gj_hostnamed_ok_u_13748  (alias)
 *   __libcgj_batch13748_marker = "libcgj-batch13748"
 *
 * Exclusive continuum CREATE-ONLY (13741-13750: dbus soft id stubs —
 * dbus_ok_u_13741, systemd_ok_u_13742, logind_ok_u_13743,
 * journal_ok_u_13744, resolved_ok_u_13745, networkd_ok_u_13746,
 * timedated_ok_u_13747, hostnamed_ok_u_13748,
 * dbus_soft_ready_u_13749, batch_id_13750). Unique surface only;
 * no multi-def. Distinct from gj_hostnamed_ok_u_10848 /
 * gj_hostnamed_ok_u_10648 / gj_hostnamed_ok_u_10448 /
 * gj_hostnamed_ok_u_10148. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13748_marker[] = "libcgj-batch13748";

/* Soft hostnamed-ok lamp: always off (not a real runtime probe). */
#define B13748_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13748_ok(void)
{
	return B13748_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_13748 - hostnamed ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * systemd-hostnamed presence or call libc. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_13748(void)
{
	(void)NULL;
	return b13748_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_13748(void)
    __attribute__((alias("gj_hostnamed_ok_u_13748")));
