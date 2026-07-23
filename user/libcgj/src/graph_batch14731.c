/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14731: continuum node pulse identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14731(void);
 *     - Returns the compile-time graph batch number for this TU (14731).
 *   uint32_t gj_graph_milestone_14731(void);
 *     - Returns the current graph milestone revision (14731).
 *   uint32_t __gj_batch_id_14731  (alias)
 *   uint32_t __gj_graph_milestone_14731  (alias)
 *   __libcgj_batch14731_marker = "libcgj-batch14731"
 *
 * Exclusive continuum CREATE-ONLY wave (14726-14750). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14731_marker[] = "libcgj-batch14731";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14731_pulse(void)
{
	return 14731u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14731 - report this TU's graph batch number.
 *
 * Always returns 14731.
 */
uint32_t
gj_batch_id_14731(void)
{
	(void)NULL;
	return b14731_pulse();
}

/*
 * gj_graph_milestone_14731 - report this TU's graph milestone revision.
 *
 * Always returns 14731. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14731(void)
{
	return b14731_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14731(void)
    __attribute__((alias("gj_batch_id_14731")));

uint32_t __gj_graph_milestone_14731(void)
    __attribute__((alias("gj_graph_milestone_14731")));
