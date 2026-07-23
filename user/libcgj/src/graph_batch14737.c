/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14737: continuum mesh pin identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14737(void);
 *     - Returns the compile-time graph batch number for this TU (14737).
 *   uint32_t gj_graph_milestone_14737(void);
 *     - Returns the current graph milestone revision (14737).
 *   uint32_t __gj_batch_id_14737  (alias)
 *   uint32_t __gj_graph_milestone_14737  (alias)
 *   __libcgj_batch14737_marker = "libcgj-batch14737"
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

const char __libcgj_batch14737_marker[] = "libcgj-batch14737";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14737_pin(void)
{
	return 14737u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14737 - report this TU's graph batch number.
 *
 * Always returns 14737.
 */
uint32_t
gj_batch_id_14737(void)
{
	(void)NULL;
	return b14737_pin();
}

/*
 * gj_graph_milestone_14737 - report this TU's graph milestone revision.
 *
 * Always returns 14737. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14737(void)
{
	return b14737_pin();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14737(void)
    __attribute__((alias("gj_batch_id_14737")));

uint32_t __gj_graph_milestone_14737(void)
    __attribute__((alias("gj_graph_milestone_14737")));
