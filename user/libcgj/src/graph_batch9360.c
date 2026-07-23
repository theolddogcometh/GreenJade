/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9360: systemd soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9360(void);
 *     - Returns the compile-time graph batch number for this TU (9360).
 *   uint32_t __gj_batch_id_9360  (alias)
 *   __libcgj_batch9360_marker = "libcgj-batch9360"
 *
 * Exclusive continuum CREATE-ONLY (9351-9360: systemd soft id stubs —
 * sd_unit_active_id_9351, sd_unit_inactive_id_9352,
 * sd_unit_failed_id_9353, sd_bus_ok_u_9354, sd_journal_ok_u_9355,
 * sd_notify_ok_u_9356, sd_listen_fds_ok_u_9357, sd_booted_ok_u_9358,
 * systemd_ready_u_9359, batch_id_9360).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9360_marker[] = "libcgj-batch9360";

#define B9360_BATCH_ID  9360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9360_id(void)
{
	return B9360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9360 - report this TU's graph batch number.
 *
 * Always returns 9360.
 */
uint32_t
gj_batch_id_9360(void)
{
	(void)NULL;
	return b9360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9360(void)
    __attribute__((alias("gj_batch_id_9360")));
