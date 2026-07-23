/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1400: milestone 1400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1400(void);
 *     — Returns the compile-time graph batch number for this TU (1400).
 *   uint32_t gj_graph_milestone_1400(void);
 *     — Returns the current graph milestone revision (1400).
 *   uint32_t __gj_batch_id_1400  (alias)
 *   uint32_t __gj_graph_milestone_1400  (alias)
 *   __libcgj_batch1400_marker = "libcgj-batch1400"
 *
 * Does NOT define gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1400_marker[] = "libcgj-batch1400";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1400 — report this TU's graph batch number.
 *
 * Always returns 1400.
 */
uint32_t
gj_batch_id_1400(void)
{
	return 1400u;
}

/*
 * gj_graph_milestone_1400 — report this TU's graph milestone revision.
 *
 * Always returns 1400 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1400(void)
{
	return 1400u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1400(void)
    __attribute__((alias("gj_batch_id_1400")));

uint32_t __gj_graph_milestone_1400(void)
    __attribute__((alias("gj_graph_milestone_1400")));
