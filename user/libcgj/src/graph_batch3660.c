/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3660: milestone 3660 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3660(void);
 *     - Returns the compile-time graph batch number for this TU (3660).
 *   uint32_t gj_graph_milestone_3660(void);
 *     - Returns the current graph milestone revision (3660).
 *   uint32_t __gj_batch_id_3660  (alias)
 *   uint32_t __gj_graph_milestone_3660  (alias)
 *   __libcgj_batch3660_marker = "libcgj-batch3660"
 *
 * Milestone for the hash exclusive wave (batches 3651-3659:
 * crc32c_update_u, crc32c_final_u, xxh32_update_u, xxh32_final_u,
 * siphash_half_u, murmur3_mix_u, city_mix_u, wyhash_mix_u,
 * seahash_mix_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3660_marker[] = "libcgj-batch3660";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3660_id(void)
{
	return 3660u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3660 - report this TU's graph batch number.
 *
 * Always returns 3660.
 */
uint32_t
gj_batch_id_3660(void)
{
	(void)NULL;
	return b3660_id();
}

/*
 * gj_graph_milestone_3660 - report this TU's graph milestone revision.
 *
 * Always returns 3660 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_3660(void)
{
	return b3660_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3660(void)
    __attribute__((alias("gj_batch_id_3660")));

uint32_t __gj_graph_milestone_3660(void)
    __attribute__((alias("gj_graph_milestone_3660")));
