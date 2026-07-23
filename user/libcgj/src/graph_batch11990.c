/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11990: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11990(void);
 *     - Returns the compile-time graph batch number for this TU (11990).
 *   uint32_t __gj_batch_id_11990  (alias)
 *   __libcgj_batch11990_marker = "libcgj-batch11990"
 *
 * Exclusive continuum CREATE-ONLY (11981-11990: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_11990 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. Distinct from gj_batch_id_11790. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11990_marker[] = "libcgj-batch11990";

#define B11990_BATCH_ID  11990u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11990_id(void)
{
	return B11990_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11990 - report this TU's graph batch number.
 *
 * Always returns 11990. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11990(void)
{
	(void)NULL;
	return b11990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11990(void)
    __attribute__((alias("gj_batch_id_11990")));
