/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2690: milestone 2690 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2690(void);
 *     - Returns the compile-time graph batch number for this TU (2690).
 *   uint32_t __gj_batch_id_2690  (alias)
 *   __libcgj_batch2690_marker = "libcgj-batch2690"
 *
 * Milestone for the UTF-8 exclusive wave (batches 2681-2689:
 * valid_lead_u, cont_count_u, codepoint_bytes_u, is_ascii_n_u,
 * count_ascii_n_u, trunc_len_u, prev_boundary_u, next_boundary_u,
 * bom_p_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2690_marker[] = "libcgj-batch2690";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2690_id(void)
{
	return 2690u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2690 - report this TU's graph batch number.
 *
 * Always returns 2690. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2690(void)
{
	(void)NULL;
	return b2690_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2690(void)
    __attribute__((alias("gj_batch_id_2690")));
