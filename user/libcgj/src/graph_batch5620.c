/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5620: MILESTONE 5620 batch identity +
 * compat-layer readiness for the Proton-GE / Wine / DXVK / VKD3D wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5620(void);
 *     - Returns the compile-time graph batch number for this TU (5620).
 *   uint32_t gj_graph_milestone_5620(void);
 *     - Returns the current graph milestone revision (5620).
 *   uint32_t gj_compat_layer_ready_5620(void);
 *     - Returns 1 (compat-layer readiness complete for wave 5620:
 *       Proton-GE version, wine prefix magic, DXVK/VKD3D readiness).
 *   uint32_t __gj_batch_id_5620  (alias)
 *   uint32_t __gj_graph_milestone_5620  (alias)
 *   uint32_t __gj_compat_layer_ready_5620  (alias)
 *   __libcgj_batch5620_marker = "libcgj-batch5620"
 *
 * MILESTONE 5620 for the exclusive continuum CREATE-ONLY wave
 * (batches 5611-5619: proton_ge_major_5611, proton_ge_minor_5612,
 * proton_ge_ver_pack_5613, wine_prefix_magic_5614,
 * wine_prefix_ready_5615, dxvk_ready_5616, dxvk_ver_hint_5617,
 * vkd3d_ready_5618, vkd3d_ver_hint_5619). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5620_marker[] = "libcgj-batch5620";

/* Compat-layer readiness lamp for milestone 5620 continuum. */
#define B5620_COMPAT_LAYER_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5620_id(void)
{
	return 5620u;
}

static uint32_t
b5620_compat_ready(void)
{
	return B5620_COMPAT_LAYER_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5620 - report this TU's graph batch number.
 *
 * Always returns 5620.
 */
uint32_t
gj_batch_id_5620(void)
{
	(void)NULL;
	return b5620_id();
}

/*
 * gj_graph_milestone_5620 - report this TU's graph milestone revision.
 *
 * Always returns 5620 (MILESTONE 5620). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5620(void)
{
	return b5620_id();
}

/*
 * gj_compat_layer_ready_5620 - report compat-layer readiness lamp.
 *
 * Always returns 1 (ready). Soft pure-data only; does not probe
 * Proton/Wine/DXVK/VKD3D installs or call libc. No parent wires.
 */
uint32_t
gj_compat_layer_ready_5620(void)
{
	(void)NULL;
	return b5620_compat_ready();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5620(void)
    __attribute__((alias("gj_batch_id_5620")));

uint32_t __gj_graph_milestone_5620(void)
    __attribute__((alias("gj_graph_milestone_5620")));

uint32_t __gj_compat_layer_ready_5620(void)
    __attribute__((alias("gj_compat_layer_ready_5620")));
