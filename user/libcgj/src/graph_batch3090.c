/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3090: batch identity (closes exclusive
 * unsigned vector reduce/scale wave 3081-3090).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3090(void);
 *     - Returns the compile-time graph batch number for this TU (3090).
 *   uint32_t __gj_batch_id_3090  (alias)
 *   __libcgj_batch3090_marker = "libcgj-batch3090"
 *
 * Closes the exclusive continuum CREATE-ONLY wave (batches 3081-3089:
 * gj_chebyshev_u64_u, gj_dot_u32_n_u, gj_dot_u64_n_u, gj_norm2_u32_n_u,
 * gj_norm2_u64_n_u, gj_scale_u32_n_u, gj_scale_u64_n_u,
 * gj_add_vec_u32_n_u, gj_add_vec_u64_n_u). Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3090_marker[] = "libcgj-batch3090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3090_id(void)
{
	return 3090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3090 - report this TU's graph batch number.
 *
 * Always returns 3090.
 */
uint32_t
gj_batch_id_3090(void)
{
	(void)NULL;
	return b3090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3090(void)
    __attribute__((alias("gj_batch_id_3090")));
