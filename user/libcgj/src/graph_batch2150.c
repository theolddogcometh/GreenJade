/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2150: milestone 2150 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2150(void);
 *     - Returns the compile-time graph batch number for this TU (2150).
 *   uint32_t gj_graph_milestone_2150(void);
 *     - Returns the current graph milestone revision (2150).
 *   uint32_t __gj_batch_id_2150  (alias)
 *   uint32_t __gj_graph_milestone_2150  (alias)
 *   __libcgj_batch2150_marker = "libcgj-batch2150"
 *
 * Milestone for the post-2140 parse exclusive wave (batches 2141-2149:
 * parse_bool_strict, parse_u32/i32/u16/u8_dec_n, format_u32/u16/u8_dec_w,
 * format_bool_lower). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2150_marker[] = "libcgj-batch2150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2150_id(void)
{
	return 2150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2150 - report this TU's graph batch number.
 *
 * Always returns 2150.
 */
uint32_t
gj_batch_id_2150(void)
{
	(void)NULL;
	return b2150_id();
}

/*
 * gj_graph_milestone_2150 - report this TU's graph milestone revision.
 *
 * Always returns 2150 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2150(void)
{
	return b2150_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2150(void)
    __attribute__((alias("gj_batch_id_2150")));

uint32_t __gj_graph_milestone_2150(void)
    __attribute__((alias("gj_graph_milestone_2150")));
