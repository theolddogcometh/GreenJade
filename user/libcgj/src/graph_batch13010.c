/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13010: continuum product deepen wave 30 batch
 * identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13010(void);
 *     - Returns the compile-time graph batch number for this TU (13010).
 *   uint32_t __gj_batch_id_13010  (alias)
 *   __libcgj_batch13010_marker = "libcgj-batch13010"
 *
 * MILESTONE 13010 for the exclusive continuum CREATE-ONLY wave
 * (batches 13001-13009: continuum_product_deepen_root_13001,
 * continuum_product_deepen_lane_13002, continuum_product_deepen_score_13003,
 * continuum_product_deepen_gate_13004, continuum_product_deepen_path_13005,
 * continuum_product_deepen_shell_13006, continuum_product_deepen_lib_13007,
 * continuum_smoke_soft_13008, continuum_dyn_soft_13009). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_12910 and gj_batch_id_12810. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13010_marker[] = "libcgj-batch13010";

#define B13010_BATCH_ID  13010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13010_id(void)
{
	return B13010_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13010 - report this TU's graph batch number.
 *
 * Always returns 13010.
 */
uint32_t
gj_batch_id_13010(void)
{
	(void)NULL;
	return b13010_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13010(void)
    __attribute__((alias("gj_batch_id_13010")));
