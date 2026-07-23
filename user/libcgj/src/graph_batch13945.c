/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13945: resolved ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolved_ok_u_13945(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-resolved runtime probe for the dbus soft continuum.
 *   uint32_t __gj_resolved_ok_u_13945  (alias)
 *   __libcgj_batch13945_marker = "libcgj-batch13945"
 *
 * Exclusive continuum CREATE-ONLY (13941-13950: dbus soft id stubs —
 * dbus_ok_u_13941, systemd_ok_u_13942, logind_ok_u_13943,
 * journal_ok_u_13944, resolved_ok_u_13945, networkd_ok_u_13946,
 * timedated_ok_u_13947, hostnamed_ok_u_13948,
 * dbus_soft_ready_u_13949, batch_id_13950→13950). Unique surface
 * only; no multi-def. Distinct from gj_resolved_ok_u_11845 /
 * gj_resolved_ok_u_11645 / gj_resolved_ok_u_10645 /
 * gj_resolved_ok_u_10445 / gj_resolved_ok_u_10145. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13945_marker[] = "libcgj-batch13945";

/* Soft resolved-ok lamp: always off (not a real runtime probe). */
#define B13945_RESOLVED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13945_ok(void)
{
	return B13945_RESOLVED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolved_ok_u_13945 - resolved ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * systemd-resolved presence or call libc. No parent wires.
 */
uint32_t
gj_resolved_ok_u_13945(void)
{
	(void)NULL;
	return b13945_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolved_ok_u_13945(void)
    __attribute__((alias("gj_resolved_ok_u_13945")));
