/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13742: systemd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_systemd_ok_u_13742(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_systemd_ok_u_13742  (alias)
 *   __libcgj_batch13742_marker = "libcgj-batch13742"
 *
 * Exclusive continuum CREATE-ONLY (13741-13750: dbus soft id stubs —
 * dbus_ok_u_13741, systemd_ok_u_13742, logind_ok_u_13743,
 * journal_ok_u_13744, resolved_ok_u_13745, networkd_ok_u_13746,
 * timedated_ok_u_13747, hostnamed_ok_u_13748,
 * dbus_soft_ready_u_13749, batch_id_13750). Unique surface only;
 * no multi-def. Distinct from gj_systemd_ok_u_10842 /
 * gj_systemd_ok_u_10642 / gj_systemd_ok_u_10442 /
 * gj_systemd_ok_u_10142. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13742_marker[] = "libcgj-batch13742";

/* Soft systemd-ok lamp: always off (not a real runtime probe). */
#define B13742_SYSTEMD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13742_ok(void)
{
	return B13742_SYSTEMD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_systemd_ok_u_13742 - systemd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_systemd_ok_u_13742(void)
{
	(void)NULL;
	return b13742_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_systemd_ok_u_13742(void)
    __attribute__((alias("gj_systemd_ok_u_13742")));
