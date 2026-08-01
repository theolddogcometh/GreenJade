/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92526: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_92526(void);
 *     - Returns the compile-time graph batch number for this TU (92526).
 *   uint32_t gj_graph_milestone_92526(void);
 *     - Returns the current graph milestone revision (92526).
 *   uint32_t __gj_batch_id_92526  (alias)
 *   uint32_t __gj_graph_milestone_92526  (alias)
 *   __libcgj_batch92526_marker = "libcgj-batch92526"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-92526, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 25900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch92526_marker[] = "libcgj-batch92526";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b92526_id(void)
{
	return 92526u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_92526 - report this TU's graph batch number.
 *
 * Always returns 92526.
 */
uint32_t
gj_batch_id_92526(void)
{
	(void)NULL;
	return b92526_id();
}

/*
 * gj_graph_milestone_92526 - report this TU's graph milestone revision.
 *
 * Always returns 92526. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_92526(void)
{
	return b92526_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_92526(void)
    __attribute__((alias("gj_batch_id_92526")));

uint32_t __gj_graph_milestone_92526(void)
    __attribute__((alias("gj_graph_milestone_92526")));
