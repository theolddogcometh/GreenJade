/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4080: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4080(void);
 *     - Returns the compile-time graph batch number for this TU (4080).
 *   uint32_t __gj_batch_id_4080  (alias)
 *   __libcgj_batch4080_marker = "libcgj-batch4080"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080): u32_tri_u,
 * u64_tri_u, u32_square_wave_u, u64_square_wave_u, u32_mod_wrap_u,
 * u64_mod_wrap_u, u32_mod_mirror_u, u64_mod_mirror_u, u32_fract_q16_u,
 * batch_id_4080. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4080_marker[] = "libcgj-batch4080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4080_id(void)
{
	return 4080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4080 - report this TU's graph batch number.
 *
 * Always returns 4080.
 */
uint32_t
gj_batch_id_4080(void)
{
	(void)NULL;
	return b4080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4080(void)
    __attribute__((alias("gj_batch_id_4080")));
