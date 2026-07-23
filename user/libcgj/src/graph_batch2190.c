/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2190: milestone 2190 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2190(void);
 *     - Returns the compile-time graph batch number for this TU (2190).
 *   uint32_t gj_graph_milestone_2190(void);
 *     - Returns the current graph milestone revision (2190).
 *   uint32_t __gj_batch_id_2190  (alias)
 *   uint32_t __gj_graph_milestone_2190  (alias)
 *   __libcgj_batch2190_marker = "libcgj-batch2190"
 *
 * Milestone for the post-2180 random exclusive wave (batches 2181-2189:
 * xorshift32_next_u, xorshift64_next_u, xorshift128_next_u,
 * splitmix64_next_u, wyhash64_seed_step, pcg32_next_u,
 * uniform_u32_bounded, bernoulli_milli_p, reservoir_accept_p).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2190_marker[] = "libcgj-batch2190";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2190_id(void)
{
	return 2190u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2190 - report this TU's graph batch number.
 *
 * Always returns 2190.
 */
uint32_t
gj_batch_id_2190(void)
{
	(void)NULL;
	return b2190_id();
}

/*
 * gj_graph_milestone_2190 - report this TU's graph milestone revision.
 *
 * Always returns 2190 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2190(void)
{
	return b2190_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2190(void)
    __attribute__((alias("gj_batch_id_2190")));

uint32_t __gj_graph_milestone_2190(void)
    __attribute__((alias("gj_graph_milestone_2190")));
