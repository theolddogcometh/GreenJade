/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10643: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_10643(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_10643  (alias)
 *   __libcgj_batch10643_marker = "libcgj-batch10643"
 *
 * Exclusive continuum CREATE-ONLY (10641-10650: dbus soft id stubs —
 * dbus_ok_u_10641, systemd_ok_u_10642, logind_ok_u_10643,
 * journal_ok_u_10644, resolved_ok_u_10645, networkd_ok_u_10646,
 * timedated_ok_u_10647, hostnamed_ok_u_10648,
 * dbus_soft_ready_u_10649, batch_id_10650). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_10443 /
 * gj_logind_ok_u_10143. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10643_marker[] = "libcgj-batch10643";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B10643_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10643_ok(void)
{
	return B10643_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_10643 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind seat
 * or session presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_10643(void)
{
	(void)NULL;
	return b10643_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_10643(void)
    __attribute__((alias("gj_logind_ok_u_10643")));
