/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4320: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4320(void);
 *     - Returns the compile-time graph batch number for this TU (4320).
 *   uint32_t __gj_batch_id_4320  (alias)
 *   __libcgj_batch4320_marker = "libcgj-batch4320"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320: u32_bit_clear_u,
 * u64_bit_clear_u, u32_bit_toggle_u, u64_bit_toggle_u, u32_bit_set_to_u,
 * u64_bit_set_to_u, u32_lowest_n_bits_u, u32_highest_n_bits_u,
 * u32_count_leading_ones_u, batch_id_4320). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4320_marker[] = "libcgj-batch4320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4320_id(void)
{
	return 4320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4320 - report this TU's graph batch number.
 *
 * Always returns 4320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4320(void)
{
	(void)NULL;
	return b4320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4320(void)
    __attribute__((alias("gj_batch_id_4320")));
