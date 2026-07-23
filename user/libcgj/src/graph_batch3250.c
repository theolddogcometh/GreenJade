/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3250: MILESTONE 3250 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3250(void);
 *     - Returns the compile-time graph batch number for this TU (3250).
 *   uint32_t gj_graph_milestone_3250(void);
 *     - Returns the current graph milestone revision (3250).
 *   uint32_t __gj_batch_id_3250  (alias)
 *   uint32_t __gj_graph_milestone_3250  (alias)
 *   __libcgj_batch3250_marker = "libcgj-batch3250"
 *
 * MILESTONE 3250 for the exclusive continuum CREATE-ONLY wave
 * (batches 3241-3249: product_wave_3250, export_hint_3250, smoke_ok_u,
 * dyn_ok_u, shell_ok_3250, libcgj_ok_3250, bar3_bits_u, bar3_full_p_u,
 * continuum_ok_3250). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3250_marker[] = "libcgj-batch3250";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3250_id(void)
{
	return 3250u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3250 - report this TU's graph batch number.
 *
 * Always returns 3250.
 */
uint32_t
gj_batch_id_3250(void)
{
	(void)NULL;
	return b3250_id();
}

/*
 * gj_graph_milestone_3250 - report this TU's graph milestone revision.
 *
 * Always returns 3250 (MILESTONE 3250). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3250(void)
{
	return b3250_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3250(void)
    __attribute__((alias("gj_batch_id_3250")));

uint32_t __gj_graph_milestone_3250(void)
    __attribute__((alias("gj_graph_milestone_3250")));
