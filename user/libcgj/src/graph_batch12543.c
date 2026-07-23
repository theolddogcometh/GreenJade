/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12543: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_12543(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_12543  (alias)
 *   __libcgj_batch12543_marker = "libcgj-batch12543"
 *
 * Exclusive continuum CREATE-ONLY (12541-12550: dbus soft id stubs —
 * dbus_ok_u_12541, systemd_ok_u_12542, logind_ok_u_12543,
 * journal_ok_u_12544, resolved_ok_u_12545, networkd_ok_u_12546,
 * timedated_ok_u_12547, hostnamed_ok_u_12548,
 * dbus_soft_ready_u_12549, batch_id_12550). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_12343 / gj_logind_ok_u_12143
 * / gj_logind_ok_u_11843.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12543_marker[] = "libcgj-batch12543";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B12543_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12543_ok(void)
{
	return B12543_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_12543 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind
 * presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_12543(void)
{
	(void)NULL;
	return b12543_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_12543(void)
    __attribute__((alias("gj_logind_ok_u_12543")));
