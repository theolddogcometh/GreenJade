/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12743: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_12743(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_12743  (alias)
 *   __libcgj_batch12743_marker = "libcgj-batch12743"
 *
 * Exclusive continuum CREATE-ONLY (12741-12750: dbus soft id stubs —
 * dbus_ok_u_12741, systemd_ok_u_12742, logind_ok_u_12743,
 * journal_ok_u_12744, resolved_ok_u_12745, networkd_ok_u_12746,
 * timedated_ok_u_12747, hostnamed_ok_u_12748,
 * dbus_soft_ready_u_12749, batch_id_12750). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_10643 /
 * gj_logind_ok_u_10443 / gj_logind_ok_u_10143. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12743_marker[] = "libcgj-batch12743";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B12743_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12743_ok(void)
{
	return B12743_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_12743 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind seat
 * or session presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_12743(void)
{
	(void)NULL;
	return b12743_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_12743(void)
    __attribute__((alias("gj_logind_ok_u_12743")));
