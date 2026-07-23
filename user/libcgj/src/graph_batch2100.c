/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2100: milestone 2100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2100(void);
 *     - Returns the compile-time graph batch number for this TU (2100).
 *   uint32_t gj_graph_milestone_2100(void);
 *     - Returns the current graph milestone revision (2100).
 *   uint32_t __gj_batch_id_2100  (alias)
 *   uint32_t __gj_graph_milestone_2100  (alias)
 *   __libcgj_batch2100_marker = "libcgj-batch2100"
 *
 * Milestone for the post-2000 id exclusive wave (batches 2091-2099:
 * id_is_nonzero_p, id_is_valid_range, gen_count_inc_sat, seq_gap_p,
 * seq_next_u32, epoch_ms_ok_hint, uuid_version_nibble, uuid_variant_ok,
 * ulid_char_ok). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2100_marker[] = "libcgj-batch2100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2100_id(void)
{
	return 2100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2100 - report this TU's graph batch number.
 *
 * Always returns 2100.
 */
uint32_t
gj_batch_id_2100(void)
{
	(void)NULL;
	return b2100_id();
}

/*
 * gj_graph_milestone_2100 - report this TU's graph milestone revision.
 *
 * Always returns 2100 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2100(void)
{
	return b2100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2100(void)
    __attribute__((alias("gj_batch_id_2100")));

uint32_t __gj_graph_milestone_2100(void)
    __attribute__((alias("gj_graph_milestone_2100")));
