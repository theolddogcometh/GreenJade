/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13143: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_13143(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_13143  (alias)
 *   __libcgj_batch13143_marker = "libcgj-batch13143"
 *
 * Exclusive continuum CREATE-ONLY (13141-13150: dbus soft id stubs —
 * dbus_ok_u_13141, systemd_ok_u_13142, logind_ok_u_13143,
 * journal_ok_u_13144, resolved_ok_u_13145, networkd_ok_u_13146,
 * timedated_ok_u_13147, hostnamed_ok_u_13148,
 * dbus_soft_ready_u_13149, batch_id_13150→13150). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_11843 / gj_logind_ok_u_11643 /
 * gj_logind_ok_u_11443. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13143_marker[] = "libcgj-batch13143";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B13143_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13143_ok(void)
{
	return B13143_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_13143 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind seat
 * or session presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_13143(void)
{
	(void)NULL;
	return b13143_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_13143(void)
    __attribute__((alias("gj_logind_ok_u_13143")));
