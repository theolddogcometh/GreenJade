/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2320: milestone 2320 batch identity (closes
 * post-2310 wrap/ovf exclusive wave 2311-2320).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2320(void);
 *     - Returns the compile-time graph batch number for this TU (2320).
 *   uint32_t gj_graph_milestone_2320(void);
 *     - Returns the current graph milestone revision (2320).
 *   uint32_t __gj_batch_id_2320  (alias)
 *   uint32_t __gj_graph_milestone_2320  (alias)
 *   __libcgj_batch2320_marker = "libcgj-batch2320"
 *
 * Milestone for the post-2310 wrap/ovf exclusive wave (batches 2311-2319:
 * u32_add_wrap, u32_sub_wrap, u32_mul_wrap, u64_add_wrap, u64_sub_wrap,
 * u64_mul_wrap_soft, u32_add_ovf_p, u64_add_ovf_p, u64_mul_ovf_p_soft).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2320_marker[] = "libcgj-batch2320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2320_id(void)
{
	return 2320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2320 - report this TU's graph batch number.
 *
 * Always returns 2320.
 */
uint32_t
gj_batch_id_2320(void)
{
	(void)NULL;
	return b2320_id();
}

/*
 * gj_graph_milestone_2320 - report this TU's graph milestone revision.
 *
 * Always returns 2320 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2320(void)
{
	return b2320_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2320(void)
    __attribute__((alias("gj_batch_id_2320")));

uint32_t __gj_graph_milestone_2320(void)
    __attribute__((alias("gj_graph_milestone_2320")));
