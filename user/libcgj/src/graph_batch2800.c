/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2800: milestone 2800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2800(void);
 *     - Returns the compile-time graph batch number for this TU (2800).
 *   uint32_t gj_graph_milestone_2800(void);
 *     - Returns the current graph milestone revision (2800).
 *   uint32_t __gj_batch_id_2800  (alias)
 *   uint32_t __gj_graph_milestone_2800  (alias)
 *   __libcgj_batch2800_marker = "libcgj-batch2800"
 *
 * Milestone for the exclusive byte/hex helpers wave (batches 2791-2799:
 * bytes_to_u32_be_u, bytes_to_u64_be_u, u32_to_bytes_be_u,
 * u64_to_bytes_be_u, u64_to_bytes_le_u, hex_nibble_u,
 * from_hex_nibble_u, smoke_ok_pair_u, dyn_ok_pair_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2800_marker[] = "libcgj-batch2800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2800_id(void)
{
	return 2800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2800 - report this TU's graph batch number.
 *
 * Always returns 2800.
 */
uint32_t
gj_batch_id_2800(void)
{
	(void)NULL;
	return b2800_id();
}

/*
 * gj_graph_milestone_2800 - report this TU's graph milestone revision.
 *
 * Always returns 2800 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2800(void)
{
	return b2800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2800(void)
    __attribute__((alias("gj_batch_id_2800")));

uint32_t __gj_graph_milestone_2800(void)
    __attribute__((alias("gj_graph_milestone_2800")));
