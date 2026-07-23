/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11443: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_11443(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_11443  (alias)
 *   __libcgj_batch11443_marker = "libcgj-batch11443"
 *
 * Exclusive continuum CREATE-ONLY (11441-11450: dbus soft id stubs —
 * dbus_ok_u_11441, systemd_ok_u_11442, logind_ok_u_11443,
 * journal_ok_u_11444, resolved_ok_u_11445, networkd_ok_u_11446,
 * timedated_ok_u_11447, hostnamed_ok_u_11448,
 * dbus_soft_ready_u_11449, batch_id_11450). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_11243 /
 * gj_logind_ok_u_11043 / gj_logind_ok_u_10843 /
 * gj_logind_ok_u_10643 / gj_logind_ok_u_10443 /
 * gj_logind_ok_u_10143. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11443_marker[] = "libcgj-batch11443";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B11443_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11443_ok(void)
{
	return B11443_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_11443 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-logind
 * presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_11443(void)
{
	(void)NULL;
	return b11443_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_11443(void)
    __attribute__((alias("gj_logind_ok_u_11443")));
