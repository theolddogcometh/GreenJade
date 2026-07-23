/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2410: milestone 2410 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2410(void);
 *     - Returns the compile-time graph batch number for this TU (2410).
 *   uint32_t gj_graph_milestone_2410(void);
 *     - Returns the current graph milestone revision (2410).
 *   uint32_t __gj_batch_id_2410  (alias)
 *   uint32_t __gj_graph_milestone_2410  (alias)
 *   __libcgj_batch2410_marker = "libcgj-batch2410"
 *
 * Milestone for the exclusive shift/rotate wave (batches 2401-2409:
 * u64_rotl_n_u, u64_rotr_n_u, u32_rotl_n_u, u32_rotr_n_u,
 * u64_shl_sat, u64_shr_sat, u32_shl_sat, u32_shr_sat,
 * u64_ashr_sat). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2410_marker[] = "libcgj-batch2410";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2410_id(void)
{
	return 2410u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2410 - report this TU's graph batch number.
 *
 * Always returns 2410.
 */
uint32_t
gj_batch_id_2410(void)
{
	(void)NULL;
	return b2410_id();
}

/*
 * gj_graph_milestone_2410 - report this TU's graph milestone revision.
 *
 * Always returns 2410 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2410(void)
{
	return b2410_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2410(void)
    __attribute__((alias("gj_batch_id_2410")));

uint32_t __gj_graph_milestone_2410(void)
    __attribute__((alias("gj_graph_milestone_2410")));
