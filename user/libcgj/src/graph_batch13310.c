/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13310: continuum product deepen wave 33 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13310(void);
 *     - Returns the compile-time graph batch number for this TU (13310).
 *   uint32_t __gj_batch_id_13310  (alias)
 *   __libcgj_batch13310_marker = "libcgj-batch13310"
 *
 * MILESTONE 13310 for the exclusive continuum CREATE-ONLY wave
 * (batches 13301-13309: continuum_product_deepen_root_13301,
 * continuum_product_deepen_lane_13302, continuum_product_deepen_score_13303,
 * continuum_product_deepen_gate_13304, continuum_product_deepen_path_13305,
 * continuum_product_deepen_shell_13306, continuum_product_deepen_lib_13307,
 * continuum_smoke_soft_13308, continuum_dyn_soft_13309). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_13210 and gj_batch_id_13110. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13310_marker[] = "libcgj-batch13310";

#define B13310_BATCH_ID  13310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13310_id(void)
{
	return B13310_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13310 - report this TU's graph batch number.
 *
 * Always returns 13310.
 */
uint32_t
gj_batch_id_13310(void)
{
	(void)NULL;
	return b13310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13310(void)
    __attribute__((alias("gj_batch_id_13310")));
