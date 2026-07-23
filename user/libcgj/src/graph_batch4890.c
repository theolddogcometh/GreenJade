/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4890: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4890(void);
 *     - Returns the compile-time graph batch number for this TU (4890).
 *   uint32_t __gj_batch_id_4890  (alias)
 *   __libcgj_batch4890_marker = "libcgj-batch4890"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4890_marker[] = "libcgj-batch4890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4890_id(void)
{
	return 4890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4890 - report this TU's graph batch number.
 *
 * Always returns 4890. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4890(void)
{
	(void)NULL;
	return b4890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4890(void)
    __attribute__((alias("gj_batch_id_4890")));
