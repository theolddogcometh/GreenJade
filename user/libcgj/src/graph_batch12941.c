/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12941: dbus ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbus_ok_u_12941(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       D-Bus runtime probe for the dbus soft continuum.
 *   uint32_t __gj_dbus_ok_u_12941  (alias)
 *   __libcgj_batch12941_marker = "libcgj-batch12941"
 *
 * Exclusive continuum CREATE-ONLY (12941-12950: dbus soft id stubs —
 * dbus_ok_u_12941, systemd_ok_u_12942, logind_ok_u_12943,
 * journal_ok_u_12944, resolved_ok_u_12945, networkd_ok_u_12946,
 * timedated_ok_u_12947, hostnamed_ok_u_12948,
 * dbus_soft_ready_u_12949, batch_id_12950). Unique surface only;
 * no multi-def. Distinct from gj_dbus_ok_u_12741 / gj_dbus_ok_u_12541 /
 * gj_dbus_ok_u_12341. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12941_marker[] = "libcgj-batch12941";

/* Soft dbus-ok lamp: always off (not a real runtime probe). */
#define B12941_DBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12941_ok(void)
{
	return B12941_DBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbus_ok_u_12941 - dbus ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe D-Bus bus
 * presence or call libc. No parent wires.
 */
uint32_t
gj_dbus_ok_u_12941(void)
{
	(void)NULL;
	return b12941_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbus_ok_u_12941(void)
    __attribute__((alias("gj_dbus_ok_u_12941")));
