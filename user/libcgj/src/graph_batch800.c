/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch800: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_800(void);
 *     — Returns the compile-time graph batch number for this TU (800).
 *   uint32_t gj_graph_milestone_800(void);
 *     — Returns the current graph milestone revision (800).
 *   uint32_t __gj_batch_id_800  (alias)
 *   uint32_t __gj_graph_milestone_800  (alias)
 *   __libcgj_batch800_marker = "libcgj-batch800"
 *
 * Does NOT define gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch800_marker[] = "libcgj-batch800";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_800 — report this TU's graph batch number.
 *
 * Always returns 800.
 */
uint32_t
gj_batch_id_800(void)
{
	return 800u;
}

/*
 * gj_graph_milestone_800 — report this TU's graph milestone revision.
 *
 * Always returns 800 (this milestone TU).
 */
uint32_t
gj_graph_milestone_800(void)
{
	return 800u;
}

uint32_t __gj_batch_id_800(void)
    __attribute__((alias("gj_batch_id_800")));

uint32_t __gj_graph_milestone_800(void)
    __attribute__((alias("gj_graph_milestone_800")));
