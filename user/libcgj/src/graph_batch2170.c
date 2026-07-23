/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2170: milestone 2170 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2170(void);
 *     - Returns the compile-time graph batch number for this TU (2170).
 *   uint32_t gj_graph_milestone_2170(void);
 *     - Returns the current graph milestone revision (2170).
 *   uint32_t __gj_batch_id_2170  (alias)
 *   uint32_t __gj_graph_milestone_2170  (alias)
 *   __libcgj_batch2170_marker = "libcgj-batch2170"
 *
 * Milestone for the post-2160 path exclusive wave (batches 2161-2169:
 * path_is_dot_p, path_is_dotdot_p, path_is_root_p, path_skip_slashes,
 * path_next_seg, path_join2_len, path_norm_dot_len, path_ext_start,
 * path_stem_start). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2170_marker[] = "libcgj-batch2170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2170_id(void)
{
	return 2170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2170 - report this TU's graph batch number.
 *
 * Always returns 2170.
 */
uint32_t
gj_batch_id_2170(void)
{
	(void)NULL;
	return b2170_id();
}

/*
 * gj_graph_milestone_2170 - report this TU's graph milestone revision.
 *
 * Always returns 2170 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2170(void)
{
	return b2170_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2170(void)
    __attribute__((alias("gj_batch_id_2170")));

uint32_t __gj_graph_milestone_2170(void)
    __attribute__((alias("gj_graph_milestone_2170")));
