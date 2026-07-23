/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14010: continuum product deepen wave 40 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14010(void);
 *     - Returns the compile-time graph batch number for this TU (14010).
 *   uint32_t __gj_batch_id_14010  (alias)
 *   __libcgj_batch14010_marker = "libcgj-batch14010"
 *
 * MILESTONE 14010 for the exclusive continuum CREATE-ONLY wave
 * (batches 14001-14009: continuum_product_deepen_root_14001,
 * continuum_product_deepen_lane_14002, continuum_product_deepen_score_14003,
 * continuum_product_deepen_gate_14004, continuum_product_deepen_path_14005,
 * continuum_product_deepen_shell_14006, continuum_product_deepen_lib_14007,
 * continuum_smoke_soft_14008, continuum_dyn_soft_14009). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14000, gj_batch_id_13010, and
 * gj_batch_id_12910. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14010_marker[] = "libcgj-batch14010";

#define B14010_BATCH_ID  14010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14010_id(void)
{
	return B14010_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14010 - report this TU's graph batch number.
 *
 * Always returns 14010.
 */
uint32_t
gj_batch_id_14010(void)
{
	(void)NULL;
	return b14010_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14010(void)
    __attribute__((alias("gj_batch_id_14010")));
