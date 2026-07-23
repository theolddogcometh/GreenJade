/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4540: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4540(void);
 *     - Returns the compile-time graph batch number for this TU (4540).
 *   uint32_t __gj_batch_id_4540  (alias)
 *   __libcgj_batch4540_marker = "libcgj-batch4540"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique —
 * u32_in_range_u, u64_in_range_u, ptr_nonnull_u, ptr_eq_u,
 * size_fits_u32_u, size_add_fits_u, size_mul_fits_u, align_ok_u,
 * u32_is_ascii_u, batch_id_4540). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4540_marker[] = "libcgj-batch4540";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4540_id(void)
{
	return 4540u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4540 - report this TU's graph batch number.
 *
 * Always returns 4540. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4540(void)
{
	(void)NULL;
	return b4540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4540(void)
    __attribute__((alias("gj_batch_id_4540")));
