/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12947: timedated ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_timedated_ok_u_12947(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-timedated runtime probe for the dbus soft continuum.
 *   uint32_t __gj_timedated_ok_u_12947  (alias)
 *   __libcgj_batch12947_marker = "libcgj-batch12947"
 *
 * Exclusive continuum CREATE-ONLY (12941-12950: dbus soft id stubs —
 * dbus_ok_u_12941, systemd_ok_u_12942, logind_ok_u_12943,
 * journal_ok_u_12944, resolved_ok_u_12945, networkd_ok_u_12946,
 * timedated_ok_u_12947, hostnamed_ok_u_12948,
 * dbus_soft_ready_u_12949, batch_id_12950). Unique surface only;
 * no multi-def. Distinct from gj_timedated_ok_u_12747 /
 * gj_timedated_ok_u_12547 / gj_timedated_ok_u_12347. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12947_marker[] = "libcgj-batch12947";

/* Soft timedated-ok lamp: always off (not a real runtime probe). */
#define B12947_TIMEDATED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12947_ok(void)
{
	return B12947_TIMEDATED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timedated_ok_u_12947 - timedated ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe timedate1 NTP
 * presence or call libc. No parent wires.
 */
uint32_t
gj_timedated_ok_u_12947(void)
{
	(void)NULL;
	return b12947_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timedated_ok_u_12947(void)
    __attribute__((alias("gj_timedated_ok_u_12947")));
