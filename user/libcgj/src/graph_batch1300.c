/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1300: milestone 1300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1300(void);
 *     — Returns the compile-time graph batch number for this TU (1300).
 *   uint32_t gj_graph_milestone_1300(void);
 *     — Returns the current graph milestone revision (1300).
 *   uint32_t __gj_batch_id_1300  (alias)
 *   uint32_t __gj_graph_milestone_1300  (alias)
 *   __libcgj_batch1300_marker = "libcgj-batch1300"
 *
 * Does NOT define gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1300_marker[] = "libcgj-batch1300";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1300 — report this TU's graph batch number.
 *
 * Always returns 1300.
 */
uint32_t
gj_batch_id_1300(void)
{
	return 1300u;
}

/*
 * gj_graph_milestone_1300 — report this TU's graph milestone revision.
 *
 * Always returns 1300 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1300(void)
{
	return 1300u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1300(void)
    __attribute__((alias("gj_batch_id_1300")));

uint32_t __gj_graph_milestone_1300(void)
    __attribute__((alias("gj_graph_milestone_1300")));
