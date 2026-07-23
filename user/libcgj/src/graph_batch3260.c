/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3260: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3260(void);
 *     - Returns the compile-time graph batch number for this TU (3260).
 *   uint32_t __gj_batch_id_3260  (alias)
 *   __libcgj_batch3260_marker = "libcgj-batch3260"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave
 * (batches 3251-3259: u32_absdiff_u, u64_absdiff_u2, u32_avg_round_u,
 * u64_avg_round_u, u32_percent_of_u, u64_percent_of_u, u32_ratio_q16_u,
 * u64_ratio_q16_u, u32_scale_q16_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3260_marker[] = "libcgj-batch3260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3260_id(void)
{
	return 3260u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3260 - report this TU's graph batch number.
 *
 * Always returns 3260. No parent wires.
 */
uint32_t
gj_batch_id_3260(void)
{
	(void)NULL;
	return b3260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3260(void)
    __attribute__((alias("gj_batch_id_3260")));
