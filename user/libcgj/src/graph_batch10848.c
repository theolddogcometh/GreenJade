/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10848: hostnamed ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_10848(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-hostnamed runtime probe for the dbus soft continuum.
 *   uint32_t __gj_hostnamed_ok_u_10848  (alias)
 *   __libcgj_batch10848_marker = "libcgj-batch10848"
 *
 * Exclusive continuum CREATE-ONLY (10841-10850: dbus soft id stubs —
 * dbus_ok_u_10841, systemd_ok_u_10842, logind_ok_u_10843,
 * journal_ok_u_10844, resolved_ok_u_10845, networkd_ok_u_10846,
 * timedated_ok_u_10847, hostnamed_ok_u_10848,
 * dbus_soft_ready_u_10849, batch_id_10850). Unique surface only;
 * no multi-def. Distinct from gj_hostnamed_ok_u_10648 /
 * gj_hostnamed_ok_u_10448 / gj_hostnamed_ok_u_10148. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10848_marker[] = "libcgj-batch10848";

/* Soft hostnamed-ok lamp: always off (not a real runtime probe). */
#define B10848_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10848_ok(void)
{
	return B10848_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_10848 - hostnamed ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * systemd-hostnamed presence or call libc. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_10848(void)
{
	(void)NULL;
	return b10848_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_10848(void)
    __attribute__((alias("gj_hostnamed_ok_u_10848")));
