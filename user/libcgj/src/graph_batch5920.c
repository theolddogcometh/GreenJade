/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5920: MILESTONE 5920 batch identity + wave
 * fingerprint for the smoke/dyn gate mirror helpers continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5920(void);
 *     - Returns the compile-time graph batch number for this TU (5920).
 *   uint32_t gj_graph_milestone_5920(void);
 *     - Returns milestone revision 5920 (end of 5911-5920 wave).
 *   uint32_t gj_product_gate_mirror_wave_5920(void);
 *     - Wave fingerprint: 0x17171720 (5911<<16 | 5920).
 *   uint32_t __gj_batch_id_5920  (alias)
 *   uint32_t __gj_graph_milestone_5920  (alias)
 *   uint32_t __gj_product_gate_mirror_wave_5920  (alias)
 *   __libcgj_batch5920_marker = "libcgj-batch5920"
 *
 * MILESTONE 5920 for the exclusive continuum CREATE-ONLY wave
 * (batches 5911-5919: smoke_gate_mirror_5911, dyn_gate_mirror_5912,
 * smoke_dyn_and_5913, smoke_dyn_or_5914, smoke_dyn_xor_5915,
 * gate_mirror_inv_5916, product_gate_pack_5917,
 * product_gate_smoke/dyn_bit_5918, product_gate_mirror_ready_5919).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5920_marker[] = "libcgj-batch5920";

#define B5920_BATCH_ID   5920u
/* Wave fingerprint: start<<16 | end  →  5911<<16 | 5920 */
#define B5920_WAVE_FP    ((5911u << 16) | 5920u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5920_id(void)
{
	return B5920_BATCH_ID;
}

static uint32_t
b5920_wave(void)
{
	return B5920_WAVE_FP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5920 - report this TU's graph batch number.
 *
 * Always returns 5920.
 */
uint32_t
gj_batch_id_5920(void)
{
	(void)NULL;
	return b5920_id();
}

/*
 * gj_graph_milestone_5920 - report this TU's graph milestone revision.
 *
 * Always returns 5920 (MILESTONE 5920). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5920(void)
{
	return b5920_id();
}

/*
 * gj_product_gate_mirror_wave_5920 - product gate-mirror wave fingerprint.
 *
 * Always returns (5911 << 16) | 5920. Soft continuum identity tag for
 * the smoke/dyn gate mirror helpers wave. No parent wires.
 */
uint32_t
gj_product_gate_mirror_wave_5920(void)
{
	return b5920_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5920(void)
    __attribute__((alias("gj_batch_id_5920")));

uint32_t __gj_graph_milestone_5920(void)
    __attribute__((alias("gj_graph_milestone_5920")));

uint32_t __gj_product_gate_mirror_wave_5920(void)
    __attribute__((alias("gj_product_gate_mirror_wave_5920")));
