/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10390: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10390(void);
 *     - Returns the compile-time graph batch number for this TU (10390).
 *   uint32_t __gj_batch_id_10390  (alias)
 *   __libcgj_batch10390_marker = "libcgj-batch10390"
 *
 * Exclusive continuum CREATE-ONLY (10381-10390: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_10390 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10390_marker[] = "libcgj-batch10390";

#define B10390_BATCH_ID  10390u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10390_id(void)
{
	return B10390_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10390 - report this TU's graph batch number.
 *
 * Always returns 10390. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10390(void)
{
	(void)NULL;
	return b10390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10390(void)
    __attribute__((alias("gj_batch_id_10390")));
