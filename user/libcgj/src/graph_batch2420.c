/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2420: milestone 2420 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2420(void);
 *     - Returns the compile-time graph batch number for this TU (2420).
 *   uint32_t gj_graph_milestone_2420(void);
 *     - Returns the current graph milestone revision (2420).
 *   uint32_t __gj_batch_id_2420  (alias)
 *   uint32_t __gj_graph_milestone_2420  (alias)
 *   __libcgj_batch2420_marker = "libcgj-batch2420"
 *
 * Milestone for the case-insensitive string / scalar ASCII fold wave
 * (batches 2411-2419: str_eq_ci_n, str_cmp_ci_n, str_starts_ci_n,
 * str_ends_ci_n, str_find_ci_n, str_count_ci_n, ascii_tolower_u,
 * ascii_toupper_u, ascii_fold_eq_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2420_marker[] = "libcgj-batch2420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2420_id(void)
{
	return 2420u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2420 - report this TU's graph batch number.
 *
 * Always returns 2420.
 */
uint32_t
gj_batch_id_2420(void)
{
	(void)NULL;
	return b2420_id();
}

/*
 * gj_graph_milestone_2420 - report this TU's graph milestone revision.
 *
 * Always returns 2420 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2420(void)
{
	return b2420_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2420(void)
    __attribute__((alias("gj_batch_id_2420")));

uint32_t __gj_graph_milestone_2420(void)
    __attribute__((alias("gj_graph_milestone_2420")));
