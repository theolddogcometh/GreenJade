/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10310: continuum product deepen wave 3 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10310(void);
 *     - Returns the compile-time graph batch number for this TU (10310).
 *   uint32_t __gj_batch_id_10310  (alias)
 *   __libcgj_batch10310_marker = "libcgj-batch10310"
 *
 * MILESTONE 10310 for the exclusive continuum CREATE-ONLY wave
 * (batches 10301-10309: continuum_product_deepen_root_10301,
 * continuum_product_deepen_lane_10302, continuum_product_deepen_score_10303,
 * continuum_product_deepen_gate_10304, continuum_product_deepen_path_10305,
 * continuum_product_deepen_shell_10306, continuum_product_deepen_lib_10307,
 * continuum_smoke_soft_10308, continuum_dyn_soft_10309). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_10110 and gj_batch_id_8210. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10310_marker[] = "libcgj-batch10310";

#define B10310_BATCH_ID  10310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10310_id(void)
{
	return B10310_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10310 - report this TU's graph batch number.
 *
 * Always returns 10310.
 */
uint32_t
gj_batch_id_10310(void)
{
	(void)NULL;
	return b10310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10310(void)
    __attribute__((alias("gj_batch_id_10310")));
