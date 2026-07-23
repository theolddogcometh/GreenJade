/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2670: milestone 2670 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2670(void);
 *     - Returns the compile-time graph batch number for this TU (2670).
 *   uint32_t __gj_batch_id_2670  (alias)
 *   __libcgj_batch2670_marker = "libcgj-batch2670"
 *
 * Milestone for the queue/stack u32 exclusive _u wave (batches 2661-2669:
 * queue init/push/pop/peek/len, stack init/push/pop/peek on caller
 * buffers). Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2670_marker[] = "libcgj-batch2670";

/* Milestone batch identity for the queue/stack u32 _u wave. */
#define B2670_BATCH_ID  2670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2670_id(void)
{
	return B2670_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2670 - report this TU's graph batch number.
 *
 * Always returns 2670. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2670(void)
{
	(void)NULL;
	return b2670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2670(void)
    __attribute__((alias("gj_batch_id_2670")));
