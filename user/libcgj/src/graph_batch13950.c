/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13950: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13950(void);
 *     - Returns the compile-time graph batch number for this TU (13950).
 *   uint32_t __gj_batch_id_13950  (alias)
 *   __libcgj_batch13950_marker = "libcgj-batch13950"
 *
 * Exclusive continuum CREATE-ONLY (13941-13950: dbus soft id stubs —
 * dbus_ok_u_13941, systemd_ok_u_13942, logind_ok_u_13943,
 * journal_ok_u_13944, resolved_ok_u_13945, networkd_ok_u_13946,
 * timedated_ok_u_13947, hostnamed_ok_u_13948,
 * dbus_soft_ready_u_13949, batch_id_13950→13950). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11850 / gj_batch_id_11650 / gj_batch_id_11450 /
 * gj_batch_id_11250 / gj_batch_id_11050 / gj_batch_id_10850 /
 * gj_batch_id_10650 / gj_batch_id_10450 / gj_batch_id_10150. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13950_marker[] = "libcgj-batch13950";

#define B13950_BATCH_ID  13950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13950_id(void)
{
	return B13950_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13950 - report this TU's graph batch number.
 *
 * Always returns 13950. Closes the 13941-13950 dbus soft continuum.
 */
uint32_t
gj_batch_id_13950(void)
{
	(void)NULL;
	return b13950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13950(void)
    __attribute__((alias("gj_batch_id_13950")));
