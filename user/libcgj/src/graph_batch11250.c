/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11250: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11250(void);
 *     - Returns the compile-time graph batch number for this TU (11250).
 *   uint32_t __gj_batch_id_11250  (alias)
 *   __libcgj_batch11250_marker = "libcgj-batch11250"
 *
 * Exclusive continuum CREATE-ONLY (11241-11250: dbus soft id stubs —
 * dbus_ok_u_11241, systemd_ok_u_11242, logind_ok_u_11243,
 * journal_ok_u_11244, resolved_ok_u_11245, networkd_ok_u_11246,
 * timedated_ok_u_11247, hostnamed_ok_u_11248,
 * dbus_soft_ready_u_11249, batch_id_11250). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11050 / gj_batch_id_10850 / gj_batch_id_10650 /
 * gj_batch_id_10450 / gj_batch_id_10150. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11250_marker[] = "libcgj-batch11250";

#define B11250_BATCH_ID  11250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11250_id(void)
{
	return B11250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11250 - report this TU's graph batch number.
 *
 * Always returns 11250.
 */
uint32_t
gj_batch_id_11250(void)
{
	(void)NULL;
	return b11250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11250(void)
    __attribute__((alias("gj_batch_id_11250")));
