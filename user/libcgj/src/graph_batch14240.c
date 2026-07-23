/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14240: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14240(void);
 *     - Returns the compile-time graph batch number for this TU (14240).
 *   uint32_t __gj_batch_id_14240  (alias)
 *   __libcgj_batch14240_marker = "libcgj-batch14240"
 *
 * Exclusive continuum CREATE-ONLY (14231-14240: dbus soft id stubs —
 * dbus_ok_u_14231, systemd_ok_u_14232, logind_ok_u_14233,
 * journal_ok_u_14234, resolved_ok_u_14235, networkd_ok_u_14236,
 * timedated_ok_u_14237, hostnamed_ok_u_14238,
 * dbus_soft_ready_u_14239, batch_id_14240). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13950 / gj_batch_id_13750 / gj_batch_id_13550 /
 * gj_batch_id_13350 / gj_batch_id_13150 / gj_batch_id_12950. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14240_marker[] = "libcgj-batch14240";

#define B14240_BATCH_ID  14240u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14240_id(void)
{
	return B14240_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14240 - report this TU's graph batch number.
 *
 * Always returns 14240. Closes the 14231-14240 dbus soft continuum.
 */
uint32_t
gj_batch_id_14240(void)
{
	(void)NULL;
	return b14240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14240(void)
    __attribute__((alias("gj_batch_id_14240")));
