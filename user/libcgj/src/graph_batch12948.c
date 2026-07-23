/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12948: hostnamed ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hostnamed_ok_u_12948(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       systemd-hostnamed runtime probe for the dbus soft continuum.
 *   uint32_t __gj_hostnamed_ok_u_12948  (alias)
 *   __libcgj_batch12948_marker = "libcgj-batch12948"
 *
 * Exclusive continuum CREATE-ONLY (12941-12950: dbus soft id stubs —
 * dbus_ok_u_12941, systemd_ok_u_12942, logind_ok_u_12943,
 * journal_ok_u_12944, resolved_ok_u_12945, networkd_ok_u_12946,
 * timedated_ok_u_12947, hostnamed_ok_u_12948,
 * dbus_soft_ready_u_12949, batch_id_12950). Unique surface only;
 * no multi-def. Distinct from gj_hostnamed_ok_u_12748 /
 * gj_hostnamed_ok_u_12548 / gj_hostnamed_ok_u_12348. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12948_marker[] = "libcgj-batch12948";

/* Soft hostnamed-ok lamp: always off (not a real runtime probe). */
#define B12948_HOSTNAMED_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12948_ok(void)
{
	return B12948_HOSTNAMED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hostnamed_ok_u_12948 - hostnamed ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe hostname1
 * presence or call libc. No parent wires.
 */
uint32_t
gj_hostnamed_ok_u_12948(void)
{
	(void)NULL;
	return b12948_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hostnamed_ok_u_12948(void)
    __attribute__((alias("gj_hostnamed_ok_u_12948")));
