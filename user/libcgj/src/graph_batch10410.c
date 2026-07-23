/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10410: continuum product deepen wave 4 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10410(void);
 *     - Returns the compile-time graph batch number for this TU (10410).
 *   uint32_t __gj_batch_id_10410  (alias)
 *   __libcgj_batch10410_marker = "libcgj-batch10410"
 *
 * MILESTONE 10410 for the exclusive continuum CREATE-ONLY wave
 * (batches 10401-10409: continuum_product_deepen_root_10401,
 * continuum_product_deepen_lane_10402, continuum_product_deepen_score_10403,
 * continuum_product_deepen_gate_10404, continuum_product_deepen_path_10405,
 * continuum_product_deepen_shell_10406, continuum_product_deepen_lib_10407,
 * continuum_smoke_soft_10408, continuum_dyn_soft_10409). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_10400, gj_batch_id_10310, and
 * gj_batch_id_10210. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10410_marker[] = "libcgj-batch10410";

#define B10410_BATCH_ID  10410u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10410_id(void)
{
	return B10410_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10410 - report this TU's graph batch number.
 *
 * Always returns 10410.
 */
uint32_t
gj_batch_id_10410(void)
{
	(void)NULL;
	return b10410_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10410(void)
    __attribute__((alias("gj_batch_id_10410")));
