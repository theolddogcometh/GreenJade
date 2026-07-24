/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21037: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21037(void);
 *     - Returns the compile-time graph batch number for this TU (21037).
 *   uint32_t gj_graph_milestone_21037(void);
 *     - Returns the current graph milestone revision (21037).
 *   uint32_t __gj_batch_id_21037  (alias)
 *   uint32_t __gj_graph_milestone_21037  (alias)
 *   __libcgj_batch21037_marker = "libcgj-batch21037"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 21001-21050, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 21100).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch21037_marker[] = "libcgj-batch21037";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21037_id(void)
{
	return 21037u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21037 - report this TU's graph batch number.
 *
 * Always returns 21037.
 */
uint32_t
gj_batch_id_21037(void)
{
	(void)NULL;
	return b21037_id();
}

/*
 * gj_graph_milestone_21037 - report this TU's graph milestone revision.
 *
 * Always returns 21037. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_21037(void)
{
	return b21037_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21037(void)
    __attribute__((alias("gj_batch_id_21037")));

uint32_t __gj_graph_milestone_21037(void)
    __attribute__((alias("gj_graph_milestone_21037")));
