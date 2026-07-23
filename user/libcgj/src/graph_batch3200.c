/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3200: MILESTONE 3200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3200(void);
 *     - Returns the compile-time graph batch number for this TU (3200).
 *   uint32_t gj_graph_milestone_3200(void);
 *     - Returns the current graph milestone revision (3200).
 *   uint32_t __gj_batch_id_3200  (alias)
 *   uint32_t __gj_graph_milestone_3200  (alias)
 *   __libcgj_batch3200_marker = "libcgj-batch3200"
 *
 * MILESTONE 3200 for the exclusive continuum CREATE-ONLY wave
 * (batches 3191-3199: buf_min_u8_u, wave_id_3200, export_hint_3200,
 * smoke_pair_ok_u, dyn_pair_ok_u, shell_ok_u, libcgj_ok_u,
 * bar3_score_3200, product_ready_3200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3200_marker[] = "libcgj-batch3200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3200_id(void)
{
return 3200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3200 - report this TU's graph batch number.
 *
 * Always returns 3200.
 */
uint32_t
gj_batch_id_3200(void)
{
(void)NULL;
return b3200_id();
}

/*
 * gj_graph_milestone_3200 - report this TU's graph milestone revision.
 *
 * Always returns 3200 (MILESTONE 3200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3200(void)
{
return b3200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3200(void)
    __attribute__((alias("gj_batch_id_3200")));

uint32_t __gj_graph_milestone_3200(void)
    __attribute__((alias("gj_graph_milestone_3200")));
