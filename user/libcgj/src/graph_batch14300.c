/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14300: MILESTONE 14300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14300(void);
 *     - Returns the compile-time graph batch number for this TU (14300).
 *   uint32_t gj_graph_milestone_14300(void);
 *     - Returns the current graph milestone revision (14300).
 *   uint32_t __gj_batch_id_14300  (alias)
 *   uint32_t __gj_graph_milestone_14300  (alias)
 *   __libcgj_batch14300_marker = "libcgj-batch14300"
 *
 * MILESTONE 14300 for the exclusive continuum CREATE-ONLY wave
 * (batches 14291-14299: shell_green_14300, libcgj_green_14300,
 * bar3_ready_14300, product_score_14300, continuum_ready_14300,
 * smoke_soft_14300, dyn_soft_14300, milestone_tag_14300,
 * continuum_wave_14300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14300_marker[] = "libcgj-batch14300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14300_id(void)
{
	return 14300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14300 - report this TU's graph batch number.
 *
 * Always returns 14300.
 */
uint32_t
gj_batch_id_14300(void)
{
	(void)NULL;
	return b14300_id();
}

/*
 * gj_graph_milestone_14300 - report this TU's graph milestone revision.
 *
 * Always returns 14300 (MILESTONE 14300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14300(void)
{
	return b14300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14300(void)
    __attribute__((alias("gj_batch_id_14300")));

uint32_t __gj_graph_milestone_14300(void)
    __attribute__((alias("gj_graph_milestone_14300")));
