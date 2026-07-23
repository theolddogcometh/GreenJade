/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10500: MILESTONE 10500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10500(void);
 *     - Returns the compile-time graph batch number for this TU (10500).
 *   uint32_t gj_graph_milestone_10500(void);
 *     - Returns the current graph milestone revision (10500).
 *   uint32_t __gj_batch_id_10500  (alias)
 *   uint32_t __gj_graph_milestone_10500  (alias)
 *   __libcgj_batch10500_marker = "libcgj-batch10500"
 *
 * MILESTONE 10500 for the exclusive continuum CREATE-ONLY wave
 * (batches 10491-10499: shell_green_10500, libcgj_green_10500,
 * bar3_ready_10500, product_score_10500, continuum_ready_10500,
 * smoke_soft_10500, dyn_soft_10500, milestone_tag_10500,
 * continuum_wave_10500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10500_marker[] = "libcgj-batch10500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10500_id(void)
{
return 10500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10500 - report this TU's graph batch number.
 *
 * Always returns 10500.
 */
uint32_t
gj_batch_id_10500(void)
{
(void)NULL;
return b10500_id();
}

/*
 * gj_graph_milestone_10500 - report this TU's graph milestone revision.
 *
 * Always returns 10500 (MILESTONE 10500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10500(void)
{
return b10500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10500(void)
    __attribute__((alias("gj_batch_id_10500")));

uint32_t __gj_graph_milestone_10500(void)
    __attribute__((alias("gj_graph_milestone_10500")));
