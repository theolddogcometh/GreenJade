/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2910: milestone 2910 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2910(void);
 *     - Returns the compile-time graph batch number for this TU (2910).
 *   uint32_t __gj_batch_id_2910  (alias)
 *   __libcgj_batch2910_marker = "libcgj-batch2910"
 *
 * Milestone for the continuum sat-arith exclusive wave (batches
 * 2901-2909: u8_sat_add_u, u8_sat_sub_u, u16_sat_add_u, u16_sat_sub_u,
 * i32_sat_add_u, i32_sat_sub_u, i64_sat_add_u, i64_sat_sub_u,
 * u32_mod_pow2_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2910_marker[] = "libcgj-batch2910";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2910_id(void)
{
	return 2910u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2910 - report this TU's graph batch number.
 *
 * Always returns 2910. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2910(void)
{
	(void)NULL;
	return b2910_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2910(void)
    __attribute__((alias("gj_batch_id_2910")));
