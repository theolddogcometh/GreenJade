/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4090: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4090(void);
 *     - Returns the compile-time graph batch number for this TU (4090).
 *   uint32_t __gj_batch_id_4090  (alias)
 *   __libcgj_batch4090_marker = "libcgj-batch4090"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090): u64_fract_q16_u,
 * u32_floor_q16_u, u64_floor_q16_u, u32_ceil_q16_u, u64_ceil_q16_u,
 * u32_round_q16_u, u64_round_q16_u, u32_frac_bits_u, u64_frac_bits_u,
 * batch_id_4090.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4090_marker[] = "libcgj-batch4090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4090_id(void)
{
	return 4090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4090 - report this TU's graph batch number.
 *
 * Always returns 4090.
 */
uint32_t
gj_batch_id_4090(void)
{
	(void)NULL;
	return b4090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4090(void)
    __attribute__((alias("gj_batch_id_4090")));
