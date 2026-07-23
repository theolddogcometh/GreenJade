/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2490: milestone 2490 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2490(void);
 *     - Returns the compile-time graph batch number for this TU (2490).
 *   uint32_t gj_graph_milestone_2490(void);
 *     - Returns the current graph milestone revision (2490).
 *   uint32_t __gj_batch_id_2490  (alias)
 *   uint32_t __gj_graph_milestone_2490  (alias)
 *   __libcgj_batch2490_marker = "libcgj-batch2490"
 *
 * Milestone for the post-2480 parse exclusive wave (batches 2481-2489:
 * parse_u64_dec_full, parse_i64_dec_full, parse_u64_hex_full,
 * parse_bool_yn, parse_bool_onoff, format_u64_dec_min, format_u64_hex_min,
 * format_u64_hex_upper_min, format_i64_dec_min). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2490_marker[] = "libcgj-batch2490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2490_id(void)
{
	return 2490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2490 - report this TU's graph batch number.
 *
 * Always returns 2490.
 */
uint32_t
gj_batch_id_2490(void)
{
	(void)NULL;
	return b2490_id();
}

/*
 * gj_graph_milestone_2490 - report this TU's graph milestone revision.
 *
 * Always returns 2490 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2490(void)
{
	return b2490_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2490(void)
    __attribute__((alias("gj_batch_id_2490")));

uint32_t __gj_graph_milestone_2490(void)
    __attribute__((alias("gj_graph_milestone_2490")));
