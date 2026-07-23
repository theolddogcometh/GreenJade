/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4860: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4860(void);
 *     - Returns the compile-time graph batch number for this TU (4860).
 *   uint32_t __gj_batch_id_4860  (alias)
 *   __libcgj_batch4860_marker = "libcgj-batch4860"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860: wg_size_u, wg_flat_u,
 * wg_x_u, wg_y_u, wg_z_u, wg_is_edge_u, wg_clamp_u, wg_div_ceil_u,
 * wg_tiles_u, batch_id_4860). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4860_marker[] = "libcgj-batch4860";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4860_id(void)
{
	return 4860u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4860 - report this TU's graph batch number.
 *
 * Always returns 4860. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4860(void)
{
	(void)NULL;
	return b4860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4860(void)
    __attribute__((alias("gj_batch_id_4860")));
