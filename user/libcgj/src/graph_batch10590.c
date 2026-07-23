/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10590: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10590(void);
 *     - Returns the compile-time graph batch number for this TU (10590).
 *   uint32_t __gj_batch_id_10590  (alias)
 *   __libcgj_batch10590_marker = "libcgj-batch10590"
 *
 * Exclusive continuum CREATE-ONLY (10581-10590: bar3 checklist soft
 * stubs, all→0). Unique gj_batch_id_10590 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Distinct from gj_batch_id_10090
 * (batch10090). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10590_marker[] = "libcgj-batch10590";

#define B10590_BATCH_ID  10590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10590_id(void)
{
	return B10590_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10590 - report this TU's graph batch number.
 *
 * Always returns 10590. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10590(void)
{
	(void)NULL;
	return b10590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10590(void)
    __attribute__((alias("gj_batch_id_10590")));
