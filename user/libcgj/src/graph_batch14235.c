/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14235: resolved ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolved_ok_u_14235(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-resolved runtime probe for the dbus soft continuum.
 *   uint32_t __gj_resolved_ok_u_14235  (alias)
 *   __libcgj_batch14235_marker = "libcgj-batch14235"
 *
 * Exclusive continuum CREATE-ONLY (14231-14240: dbus soft id stubs —
 * dbus_ok_u_14231, systemd_ok_u_14232, logind_ok_u_14233,
 * journal_ok_u_14234, resolved_ok_u_14235, networkd_ok_u_14236,
 * timedated_ok_u_14237, hostnamed_ok_u_14238,
 * dbus_soft_ready_u_14239, batch_id_14240). Unique surface only;
 * no multi-def. Distinct from gj_resolved_ok_u_13945 /
 * gj_resolved_ok_u_13745 / gj_resolved_ok_u_13545 /
 * gj_resolved_ok_u_13345 / gj_resolved_ok_u_13145 /
 * gj_resolved_ok_u_12945. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14235_marker[] = "libcgj-batch14235";

/* Soft resolved-ok lamp: always off (not a real runtime probe). */
#define B14235_RESOLVED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14235_ok(void)
{
	return B14235_RESOLVED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolved_ok_u_14235 - resolved ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe systemd-resolved
 * presence or call libc. No parent wires.
 */
uint32_t
gj_resolved_ok_u_14235(void)
{
	(void)NULL;
	return b14235_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolved_ok_u_14235(void)
    __attribute__((alias("gj_resolved_ok_u_14235")));
