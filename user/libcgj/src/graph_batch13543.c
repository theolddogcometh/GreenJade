/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13543: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_13543(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_13543  (alias)
 *   __libcgj_batch13543_marker = "libcgj-batch13543"
 *
 * Exclusive continuum CREATE-ONLY (13541-13550: dbus soft id stubs —
 * dbus_ok_u_13541, systemd_ok_u_13542, logind_ok_u_13543,
 * journal_ok_u_13544, resolved_ok_u_13545, networkd_ok_u_13546,
 * timedated_ok_u_13547, hostnamed_ok_u_13548,
 * dbus_soft_ready_u_13549, batch_id_13550→13550). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_13343 / gj_logind_ok_u_13143 /
 * gj_logind_ok_u_12943 / gj_logind_ok_u_12743. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13543_marker[] = "libcgj-batch13543";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B13543_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13543_ok(void)
{
	return B13543_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_13543 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind
 * presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_13543(void)
{
	(void)NULL;
	return b13543_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_13543(void)
    __attribute__((alias("gj_logind_ok_u_13543")));
