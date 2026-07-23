/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1900: milestone 1900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1900(void);
 *     — Returns the compile-time graph batch number for this TU (1900).
 *   uint32_t gj_graph_milestone_1900(void);
 *     — Returns the current graph milestone revision (1900).
 *   uint32_t __gj_batch_id_1900  (alias)
 *   uint32_t __gj_graph_milestone_1900  (alias)
 *   __libcgj_batch1900_marker = "libcgj-batch1900"
 *
 * Milestone for the syscall/ABI exclusive pure-data wave
 * (batches 1891–1899). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1900_marker[] = "libcgj-batch1900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1900_id(void)
{
	return 1900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1900 — report this TU's graph batch number.
 *
 * Always returns 1900.
 */
uint32_t
gj_batch_id_1900(void)
{
	(void)NULL;
	return b1900_id();
}

/*
 * gj_graph_milestone_1900 — report this TU's graph milestone revision.
 *
 * Always returns 1900 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1900(void)
{
	return b1900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1900(void)
    __attribute__((alias("gj_batch_id_1900")));

uint32_t __gj_graph_milestone_1900(void)
    __attribute__((alias("gj_graph_milestone_1900")));
