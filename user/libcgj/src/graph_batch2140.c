/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2140: milestone 2140 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2140(void);
 *     - Returns the compile-time graph batch number for this TU (2140).
 *   uint32_t gj_graph_milestone_2140(void);
 *     - Returns the current graph milestone revision (2140).
 *   uint32_t __gj_batch_id_2140  (alias)
 *   uint32_t __gj_graph_milestone_2140  (alias)
 *   __libcgj_batch2140_marker = "libcgj-batch2140"
 *
 * Milestone for the post-2130 bit reverse exclusive wave (batches
 * 2131-2139: u8/u16/u32/u64 bit reverse, u32/u64 byte reverse,
 * u32/u64 nibble reverse, u32 gray_to_bin). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2140_marker[] = "libcgj-batch2140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2140_id(void)
{
	return 2140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2140 - report this TU's graph batch number.
 *
 * Always returns 2140.
 */
uint32_t
gj_batch_id_2140(void)
{
	(void)NULL;
	return b2140_id();
}

/*
 * gj_graph_milestone_2140 - report this TU's graph milestone revision.
 *
 * Always returns 2140 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2140(void)
{
	return b2140_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2140(void)
    __attribute__((alias("gj_batch_id_2140")));

uint32_t __gj_graph_milestone_2140(void)
    __attribute__((alias("gj_graph_milestone_2140")));
