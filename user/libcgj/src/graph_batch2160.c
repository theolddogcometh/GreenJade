/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2160: milestone 2160 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2160(void);
 *     - Returns the compile-time graph batch number for this TU (2160).
 *   uint32_t gj_graph_milestone_2160(void);
 *     - Returns the current graph milestone revision (2160).
 *   uint32_t __gj_batch_id_2160  (alias)
 *   uint32_t __gj_graph_milestone_2160  (alias)
 *   __libcgj_batch2160_marker = "libcgj-batch2160"
 *
 * Milestone for the post-2150 atomics-hint exclusive pure-volatile wave
 * (batches 2151-2159: flag init/test_and_set/clear, u32 load/store/add/
 * sub/cas, u64 load). Documented NON-ATOMIC cooperative semantics only;
 * no real atomics in that wave.
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2160_marker[] = "libcgj-batch2160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2160_id(void)
{
	return 2160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2160 - report this TU's graph batch number.
 *
 * Always returns 2160.
 */
uint32_t
gj_batch_id_2160(void)
{
	(void)NULL;
	return b2160_id();
}

/*
 * gj_graph_milestone_2160 - report this TU's graph milestone revision.
 *
 * Always returns 2160 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2160(void)
{
	return b2160_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2160(void)
    __attribute__((alias("gj_batch_id_2160")));

uint32_t __gj_graph_milestone_2160(void)
    __attribute__((alias("gj_graph_milestone_2160")));
