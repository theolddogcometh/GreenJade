/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12342: systemd ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_systemd_ok_u_12342(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd runtime probe for the dbus soft continuum.
 *   uint32_t __gj_systemd_ok_u_12342  (alias)
 *   __libcgj_batch12342_marker = "libcgj-batch12342"
 *
 * Exclusive continuum CREATE-ONLY (12341-12350: dbus soft id stubs —
 * dbus_ok_u_12341, systemd_ok_u_12342, logind_ok_u_12343,
 * journal_ok_u_12344, resolved_ok_u_12345, networkd_ok_u_12346,
 * timedated_ok_u_12347, hostnamed_ok_u_12348,
 * dbus_soft_ready_u_12349, batch_id_12350). Unique surface only;
 * no multi-def. Distinct from gj_systemd_ok_u_12142 / gj_systemd_ok_u_11842
 * / gj_systemd_ok_u_11642.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12342_marker[] = "libcgj-batch12342";

/* Soft systemd-ok lamp: always off (not a real runtime probe). */
#define B12342_SYSTEMD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12342_ok(void)
{
	return B12342_SYSTEMD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_systemd_ok_u_12342 - systemd ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd
 * presence or call libc. No parent wires.
 */
uint32_t
gj_systemd_ok_u_12342(void)
{
	(void)NULL;
	return b12342_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_systemd_ok_u_12342(void)
    __attribute__((alias("gj_systemd_ok_u_12342")));
