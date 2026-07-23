/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2060: milestone 2060 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2060(void);
 *     - Returns the compile-time graph batch number for this TU (2060).
 *   uint32_t gj_graph_milestone_2060(void);
 *     - Returns the current graph milestone revision (2060).
 *   uint32_t __gj_batch_id_2060  (alias)
 *   uint32_t __gj_graph_milestone_2060  (alias)
 *   __libcgj_batch2060_marker = "libcgj-batch2060"
 *
 * Milestone for the post-2050 hash-mix exclusive wave (batches 2051-2059:
 * mix32_u, mix64_u, combine32_u, combine64_u, avalanche32, avalanche64,
 * fib_mul32, fib_mul64, xorshift_fold64). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2060_marker[] = "libcgj-batch2060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2060_id(void)
{
	return 2060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2060 - report this TU's graph batch number.
 *
 * Always returns 2060.
 */
uint32_t
gj_batch_id_2060(void)
{
	(void)NULL;
	return b2060_id();
}

/*
 * gj_graph_milestone_2060 - report this TU's graph milestone revision.
 *
 * Always returns 2060 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2060(void)
{
	return b2060_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2060(void)
    __attribute__((alias("gj_batch_id_2060")));

uint32_t __gj_graph_milestone_2060(void)
    __attribute__((alias("gj_graph_milestone_2060")));
