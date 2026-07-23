/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2550: milestone 2550 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2550(void);
 *     - Returns the compile-time graph batch number for this TU (2550).
 *   uint32_t __gj_batch_id_2550  (alias)
 *   __libcgj_batch2550_marker = "libcgj-batch2550"
 *
 * Milestone for the sync exclusive pure-volatile hints wave
 * (batches 2541-2549: flag_set/clear/test/test_and_set, counter
 * inc/dec/add/cas, seq_next_u64). Documented NON-ATOMIC cooperative
 * semantics only; no real atomics in that wave.
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2550_marker[] = "libcgj-batch2550";

/* Milestone batch identity for the sync pure-volatile wave. */
#define B2550_BATCH_ID  2550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2550_id(void)
{
	return B2550_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2550 - report this TU's graph batch number.
 *
 * Always returns 2550. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2550(void)
{
	(void)NULL;
	return b2550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2550(void)
    __attribute__((alias("gj_batch_id_2550")));
