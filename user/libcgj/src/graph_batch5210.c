/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5210: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5210(void);
 *     - Returns the compile-time graph batch number for this TU (5210).
 *   uint32_t __gj_batch_id_5210  (alias)
 *   __libcgj_batch5210_marker = "libcgj-batch5210"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique —
 * hpet_period_fs_default_u, hpet_fs_to_ns_u, hpet_ns_to_ticks_u,
 * hpet_ticks_to_ns_u, timer_hz_to_period_ns_u, timer_period_ns_to_hz_u,
 * timer_deadline_u, timer_overdue_u, timer_slack_u, batch_id_5210).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5210_marker[] = "libcgj-batch5210";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5210_id(void)
{
	return 5210u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5210 - report this TU's graph batch number.
 *
 * Always returns 5210. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5210(void)
{
	(void)NULL;
	return b5210_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5210(void)
    __attribute__((alias("gj_batch_id_5210")));
