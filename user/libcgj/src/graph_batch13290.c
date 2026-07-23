/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13290: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13290(void);
 *     - Returns the compile-time graph batch number for this TU (13290).
 *   uint32_t __gj_batch_id_13290  (alias)
 *   __libcgj_batch13290_marker = "libcgj-batch13290"
 *
 * Exclusive continuum CREATE-ONLY (13281-13290: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_13290 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. Distinct from gj_batch_id_13090. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13290_marker[] = "libcgj-batch13290";

#define B13290_BATCH_ID  13290u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13290_id(void)
{
	return B13290_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13290 - report this TU's graph batch number.
 *
 * Always returns 13290. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13290(void)
{
	(void)NULL;
	return b13290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13290(void)
    __attribute__((alias("gj_batch_id_13290")));
