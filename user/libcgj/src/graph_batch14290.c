/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14290: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14290(void);
 *     - Returns the compile-time graph batch number for this TU (14290).
 *   uint32_t __gj_batch_id_14290  (alias)
 *   __libcgj_batch14290_marker = "libcgj-batch14290"
 *
 * Exclusive continuum CREATE-ONLY (14281-14290: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_14290 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. Distinct from gj_batch_id_14190. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14290_marker[] = "libcgj-batch14290";

#define B14290_BATCH_ID  14290u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14290_id(void)
{
	return B14290_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14290 - report this TU's graph batch number.
 *
 * Always returns 14290. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_14290(void)
{
	(void)NULL;
	return b14290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14290(void)
    __attribute__((alias("gj_batch_id_14290")));
