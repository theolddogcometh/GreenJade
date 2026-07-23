/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14310: continuum product deepen wave 43 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14310(void);
 *     - Returns the compile-time graph batch number for this TU (14310).
 *   uint32_t __gj_batch_id_14310  (alias)
 *   __libcgj_batch14310_marker = "libcgj-batch14310"
 *
 * MILESTONE 14310 for the exclusive continuum CREATE-ONLY wave
 * (batches 14301-14309: continuum_product_deepen_root_14301,
 * continuum_product_deepen_lane_14302, continuum_product_deepen_score_14303,
 * continuum_product_deepen_gate_14304, continuum_product_deepen_path_14305,
 * continuum_product_deepen_shell_14306, continuum_product_deepen_lib_14307,
 * continuum_smoke_soft_14308, continuum_dyn_soft_14309). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14010, gj_batch_id_14000, and
 * gj_batch_id_13010. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14310_marker[] = "libcgj-batch14310";

#define B14310_BATCH_ID  14310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14310_id(void)
{
	return B14310_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14310 - report this TU's graph batch number.
 *
 * Always returns 14310.
 */
uint32_t
gj_batch_id_14310(void)
{
	(void)NULL;
	return b14310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14310(void)
    __attribute__((alias("gj_batch_id_14310")));
