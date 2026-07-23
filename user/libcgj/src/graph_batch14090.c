/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14090: continuum product deepen + soft
 * smoke/dyn batch identity (post-14080).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14090(void);
 *     - Returns the compile-time graph batch number for this TU (14090).
 *   uint32_t __gj_batch_id_14090  (alias)
 *   __libcgj_batch14090_marker = "libcgj-batch14090"
 *
 * MILESTONE 14090 for the exclusive continuum CREATE-ONLY wave
 * (batches 14081-14089: continuum_product_deepen_root_14081,
 * continuum_product_deepen_lane_14082, continuum_product_deepen_score_14083,
 * continuum_product_deepen_gate_14084, continuum_product_deepen_path_14085,
 * continuum_product_deepen_shell_14086, continuum_product_deepen_lib_14087,
 * continuum_smoke_soft_14088, continuum_dyn_soft_14089). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14010 (wave 40), gj_batch_id_14000, and
 * gj_batch_id_13910. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14090_marker[] = "libcgj-batch14090";

#define B14090_BATCH_ID  14090u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14090_id(void)
{
	return B14090_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14090 - report this TU's graph batch number.
 *
 * Always returns 14090.
 */
uint32_t
gj_batch_id_14090(void)
{
	(void)NULL;
	return b14090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14090(void)
    __attribute__((alias("gj_batch_id_14090")));
