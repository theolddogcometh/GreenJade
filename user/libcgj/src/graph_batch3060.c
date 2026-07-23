/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3060: milestone 3060 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3060(void);
 *     - Returns the compile-time graph batch number for this TU (3060).
 *   uint32_t __gj_batch_id_3060  (alias)
 *   __libcgj_batch3060_marker = "libcgj-batch3060"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave
 * (batches 3051-3059: u32_mul_div_u, u64_mul_div_u, u32_pct_u,
 * u64_pct_u, lerp_u32_u, lerp_u64_u, smoothstep_q16_u, clamp01_q16_u,
 * q16_mul_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3060_marker[] = "libcgj-batch3060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3060_id(void)
{
	return 3060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3060 - report this TU's graph batch number.
 *
 * Always returns 3060. Link-time presence of this symbol tags the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3060(void)
{
	(void)NULL;
	return b3060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3060(void)
    __attribute__((alias("gj_batch_id_3060")));
