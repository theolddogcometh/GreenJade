/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2030: milestone 2030 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2030(void);
 *     — Returns the compile-time graph batch number for this TU (2030).
 *   uint32_t gj_graph_milestone_2030(void);
 *     — Returns the current graph milestone revision (2030).
 *   uint32_t __gj_batch_id_2030  (alias)
 *   uint32_t __gj_graph_milestone_2030  (alias)
 *   __libcgj_batch2030_marker = "libcgj-batch2030"
 *
 * Milestone for the post-2000 mem exclusive wave (batches 2021–2029:
 * is_zero_n, is_ff_n, count_byte_n, first_nonzero, last_nonzero,
 * eq_byte_n, or_bytes, andnot_bytes, swap_bytes). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2030_marker[] = "libcgj-batch2030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2030_id(void)
{
	return 2030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2030 — report this TU's graph batch number.
 *
 * Always returns 2030.
 */
uint32_t
gj_batch_id_2030(void)
{
	(void)NULL;
	return b2030_id();
}

/*
 * gj_graph_milestone_2030 — report this TU's graph milestone revision.
 *
 * Always returns 2030 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2030(void)
{
	return b2030_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2030(void)
    __attribute__((alias("gj_batch_id_2030")));

uint32_t __gj_graph_milestone_2030(void)
    __attribute__((alias("gj_graph_milestone_2030")));
