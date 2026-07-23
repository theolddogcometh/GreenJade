/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3540: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3540(void);
 *     - Returns the compile-time graph batch number for this TU (3540).
 *   uint32_t __gj_batch_id_3540  (alias)
 *   __libcgj_batch3540_marker = "libcgj-batch3540"
 *
 * Exclusive continuum CREATE-ONLY wave (3531-3540:
 * is_sorted_strict_u64_u, arr_u32_unique_count_u, arr_u64_unique_count_u,
 * arr_u32_mode_u, arr_u32_median_u, arr_u64_median_u,
 * arr_u32_partition_u, arr_u64_partition_u, arr_u32_nth_element_u,
 * batch_id_3540). Distinct from prior batch_id_* symbols — unique
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3540_marker[] = "libcgj-batch3540";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3540_id(void)
{
	return 3540u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3540 - report this TU's graph batch number.
 *
 * Always returns 3540. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3540(void)
{
	(void)NULL;
	return b3540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3540(void)
    __attribute__((alias("gj_batch_id_3540")));
