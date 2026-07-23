/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11390: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11390(void);
 *     - Returns the compile-time graph batch number for this TU (11390).
 *   uint32_t __gj_batch_id_11390  (alias)
 *   __libcgj_batch11390_marker = "libcgj-batch11390"
 *
 * Exclusive continuum CREATE-ONLY (11381-11390: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_11390 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. Distinct from gj_batch_id_11190. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11390_marker[] = "libcgj-batch11390";

#define B11390_BATCH_ID  11390u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11390_id(void)
{
	return B11390_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11390 - report this TU's graph batch number.
 *
 * Always returns 11390. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11390(void)
{
	(void)NULL;
	return b11390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11390(void)
    __attribute__((alias("gj_batch_id_11390")));
