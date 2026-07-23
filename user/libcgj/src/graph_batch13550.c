/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13550: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13550(void);
 *     - Returns the compile-time graph batch number for this TU (13550).
 *   uint32_t __gj_batch_id_13550  (alias)
 *   __libcgj_batch13550_marker = "libcgj-batch13550"
 *
 * Exclusive continuum CREATE-ONLY (13541-13550: dbus soft id stubs —
 * dbus_ok_u_13541, systemd_ok_u_13542, logind_ok_u_13543,
 * journal_ok_u_13544, resolved_ok_u_13545, networkd_ok_u_13546,
 * timedated_ok_u_13547, hostnamed_ok_u_13548,
 * dbus_soft_ready_u_13549, batch_id_13550→13550). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13350 / gj_batch_id_13150 / gj_batch_id_12950 /
 * gj_batch_id_12750. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13550_marker[] = "libcgj-batch13550";

#define B13550_BATCH_ID  13550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13550_id(void)
{
	return B13550_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13550 - report this TU's graph batch number.
 *
 * Always returns 13550. Closes the 13541-13550 dbus soft wave.
 */
uint32_t
gj_batch_id_13550(void)
{
	(void)NULL;
	return b13550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13550(void)
    __attribute__((alias("gj_batch_id_13550")));
