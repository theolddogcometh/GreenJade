/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14233: logind ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_ok_u_14233(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       logind runtime probe for the dbus soft continuum.
 *   uint32_t __gj_logind_ok_u_14233  (alias)
 *   __libcgj_batch14233_marker = "libcgj-batch14233"
 *
 * Exclusive continuum CREATE-ONLY (14231-14240: dbus soft id stubs —
 * dbus_ok_u_14231, systemd_ok_u_14232, logind_ok_u_14233,
 * journal_ok_u_14234, resolved_ok_u_14235, networkd_ok_u_14236,
 * timedated_ok_u_14237, hostnamed_ok_u_14238,
 * dbus_soft_ready_u_14239, batch_id_14240). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_13943 /
 * gj_logind_ok_u_13743 / gj_logind_ok_u_13543 /
 * gj_logind_ok_u_13343 / gj_logind_ok_u_13143 /
 * gj_logind_ok_u_12943. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14233_marker[] = "libcgj-batch14233";

/* Soft logind-ok lamp: always off (not a real runtime probe). */
#define B14233_LOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14233_ok(void)
{
	return B14233_LOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_ok_u_14233 - logind ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind
 * presence or call libc. No parent wires.
 */
uint32_t
gj_logind_ok_u_14233(void)
{
	(void)NULL;
	return b14233_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_ok_u_14233(void)
    __attribute__((alias("gj_logind_ok_u_14233")));
