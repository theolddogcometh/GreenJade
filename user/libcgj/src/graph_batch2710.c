/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2710: milestone 2710 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2710(void);
 *     - Returns the compile-time graph batch number for this TU (2710).
 *   uint32_t gj_graph_milestone_2710(void);
 *     - Returns the current graph milestone revision (2710).
 *   uint32_t __gj_batch_id_2710  (alias)
 *   uint32_t __gj_graph_milestone_2710  (alias)
 *   __libcgj_batch2710_marker = "libcgj-batch2710"
 *
 * Milestone for the exclusive product/Steam integer helpers wave
 * (batches 2701-2709: u8_clamp_u, u16_clamp_u, u32_clamp_u,
 * u64_clamp_soft_u, i32_abs_u, i64_abs_mag_u, u32_add_sat_u,
 * u32_sub_sat_u, u64_sat_add_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2710_marker[] = "libcgj-batch2710";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2710_id(void)
{
	return 2710u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2710 - report this TU's graph batch number.
 *
 * Always returns 2710.
 */
uint32_t
gj_batch_id_2710(void)
{
	(void)NULL;
	return b2710_id();
}

/*
 * gj_graph_milestone_2710 - report this TU's graph milestone revision.
 *
 * Always returns 2710 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2710(void)
{
	return b2710_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2710(void)
    __attribute__((alias("gj_batch_id_2710")));

uint32_t __gj_graph_milestone_2710(void)
    __attribute__((alias("gj_graph_milestone_2710")));
