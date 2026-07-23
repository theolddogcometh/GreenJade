/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3780: milestone 3780 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3780(void);
 *     - Returns the compile-time graph batch number for this TU (3780).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3771-3779: kmp_prefix_u / kmp_step_u / boyer_skip_u /
 *       rabin_hash_u / rabin_roll_u / lcs_cell_u / edit_cell_u /
 *       lcs_len_step_u / z_box_u).
 *   uint32_t __gj_batch_id_3780  (alias)
 *   __libcgj_batch3780_marker = "libcgj-batch3780"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3780_marker[] = "libcgj-batch3780";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3780_id(void)
{
	return 3780u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3780 — report this TU's graph batch number.
 *
 * Always returns 3780. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3780(void)
{
	(void)NULL;
	return b3780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3780(void)
    __attribute__((alias("gj_batch_id_3780")));
