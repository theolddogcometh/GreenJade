/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14750: continuum wave close identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14750(void);
 *     - Returns the compile-time graph batch number for this TU (14750).
 *   uint32_t gj_graph_milestone_14750(void);
 *     - Returns the current graph milestone revision (14750).
 *   uint32_t __gj_batch_id_14750  (alias)
 *   uint32_t __gj_graph_milestone_14750  (alias)
 *   __libcgj_batch14750_marker = "libcgj-batch14750"
 *
 * Exclusive continuum CREATE-ONLY wave close (14726-14750). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14750_marker[] = "libcgj-batch14750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14750_close(void)
{
	return 14750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14750 - report this TU's graph batch number.
 *
 * Always returns 14750.
 */
uint32_t
gj_batch_id_14750(void)
{
	(void)NULL;
	return b14750_close();
}

/*
 * gj_graph_milestone_14750 - report this TU's graph milestone revision.
 *
 * Always returns 14750. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14750(void)
{
	return b14750_close();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14750(void)
    __attribute__((alias("gj_batch_id_14750")));

uint32_t __gj_graph_milestone_14750(void)
    __attribute__((alias("gj_graph_milestone_14750")));
