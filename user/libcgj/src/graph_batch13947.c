/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13947: timedated ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_timedated_ok_u_13947(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-timedated runtime probe for the dbus soft continuum.
 *   uint32_t __gj_timedated_ok_u_13947  (alias)
 *   __libcgj_batch13947_marker = "libcgj-batch13947"
 *
 * Exclusive continuum CREATE-ONLY (13941-13950: dbus soft id stubs —
 * dbus_ok_u_13941, systemd_ok_u_13942, logind_ok_u_13943,
 * journal_ok_u_13944, resolved_ok_u_13945, networkd_ok_u_13946,
 * timedated_ok_u_13947, hostnamed_ok_u_13948,
 * dbus_soft_ready_u_13949, batch_id_13950→13950). Unique surface
 * only; no multi-def. Distinct from gj_timedated_ok_u_11847 /
 * gj_timedated_ok_u_11647 / gj_timedated_ok_u_10647 /
 * gj_timedated_ok_u_10447 / gj_timedated_ok_u_10147. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13947_marker[] = "libcgj-batch13947";

/* Soft timedated-ok lamp: always off (not a real runtime probe). */
#define B13947_TIMEDATED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13947_ok(void)
{
	return B13947_TIMEDATED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timedated_ok_u_13947 - timedated ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * systemd-timedated presence or call libc. No parent wires.
 */
uint32_t
gj_timedated_ok_u_13947(void)
{
	(void)NULL;
	return b13947_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timedated_ok_u_13947(void)
    __attribute__((alias("gj_timedated_ok_u_13947")));
