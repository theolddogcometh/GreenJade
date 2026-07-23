/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13410: continuum product deepen wave 34 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13410(void);
 *     - Returns the compile-time graph batch number for this TU (13410).
 *   uint32_t __gj_batch_id_13410  (alias)
 *   __libcgj_batch13410_marker = "libcgj-batch13410"
 *
 * MILESTONE 13410 for the exclusive continuum CREATE-ONLY wave
 * (batches 13401-13409: continuum_product_deepen_root_13401,
 * continuum_product_deepen_lane_13402, continuum_product_deepen_score_13403,
 * continuum_product_deepen_gate_13404, continuum_product_deepen_path_13405,
 * continuum_product_deepen_shell_13406, continuum_product_deepen_lib_13407,
 * continuum_smoke_soft_13408, continuum_dyn_soft_13409). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_13310 and gj_batch_id_13210. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13410_marker[] = "libcgj-batch13410";

#define B13410_BATCH_ID  13410u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13410_id(void)
{
	return B13410_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13410 - report this TU's graph batch number.
 *
 * Always returns 13410.
 */
uint32_t
gj_batch_id_13410(void)
{
	(void)NULL;
	return b13410_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13410(void)
    __attribute__((alias("gj_batch_id_13410")));
