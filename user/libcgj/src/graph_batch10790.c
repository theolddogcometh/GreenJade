/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10790: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10790(void);
 *     - Returns the compile-time graph batch number for this TU (10790).
 *   uint32_t __gj_batch_id_10790  (alias)
 *   __libcgj_batch10790_marker = "libcgj-batch10790"
 *
 * Exclusive continuum CREATE-ONLY (10781-10790: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_10790 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Distinct from gj_batch_id_10590
 * (batch10590). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10790_marker[] = "libcgj-batch10790";

#define B10790_BATCH_ID  10790u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10790_id(void)
{
	return B10790_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10790 - report this TU's graph batch number.
 *
 * Always returns 10790. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10790(void)
{
	(void)NULL;
	return b10790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10790(void)
    __attribute__((alias("gj_batch_id_10790")));
