/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4380: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4380(void);
 *     - Returns the compile-time graph batch number for this TU (4380).
 *   uint32_t __gj_batch_id_4380  (alias)
 *   __libcgj_batch4380_marker = "libcgj-batch4380"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380: ms_to_us_u, us_to_ms_u,
 * sec_to_ms_u, ms_to_sec_u, ns_to_us_u, us_to_ns_sat_u,
 * hz_to_period_ns_u, period_ns_to_hz_u, ticks_to_ns_u, batch_id_4380).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4380_marker[] = "libcgj-batch4380";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4380_id(void)
{
	return 4380u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4380 - report this TU's graph batch number.
 *
 * Always returns 4380. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4380(void)
{
	(void)NULL;
	return b4380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4380(void)
    __attribute__((alias("gj_batch_id_4380")));
