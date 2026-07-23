/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3970: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3970(void);
 *     - Returns the compile-time graph batch number for this TU (3970).
 *   uint32_t __gj_batch_id_3970  (alias)
 *   __libcgj_batch3970_marker = "libcgj-batch3970"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970): u64_gray_prev_u,
 * u32_hamming_wt_u, u64_hamming_wt_u, u32_hamming_dist_u,
 * u64_hamming_dist_u, u32_leading_ones_u, u64_leading_ones_u,
 * u32_trailing_ones_u, u64_trailing_ones_u, batch_id_3970.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3970_marker[] = "libcgj-batch3970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3970_id(void)
{
	return 3970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3970 - report this TU's graph batch number.
 *
 * Always returns 3970.
 */
uint32_t
gj_batch_id_3970(void)
{
	(void)NULL;
	return b3970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3970(void)
    __attribute__((alias("gj_batch_id_3970")));
