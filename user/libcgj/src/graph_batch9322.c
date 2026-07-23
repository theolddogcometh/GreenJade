/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9322: GStreamer READY state soft id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gst_state_ready_id_9322(void);
 *     - Returns 2 (GST_STATE_READY soft catalog value). Pure-data state
 *       id stub; does not set element state or call GStreamer/libc.
 *   uint32_t __gj_gst_state_ready_id_9322  (alias)
 *   __libcgj_batch9322_marker = "libcgj-batch9322"
 *
 * Exclusive continuum CREATE-ONLY (9321-9330: gstreamer soft id stubs —
 * gst_state_null_id_9321, gst_state_ready_id_9322,
 * gst_state_paused_id_9323, gst_state_playing_id_9324,
 * gst_pipeline_ok_u_9325, gst_element_ok_u_9326, gst_bus_ok_u_9327,
 * gst_caps_ok_u_9328, gstreamer_ready_u_9329, batch_id_9330).
 * Unique surface only; no multi-def. Distinct from
 * gj_gst_state_null_id_9321 (batch9321). No parent wires. No __int128.
 * No GStreamer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9322_marker[] = "libcgj-batch9322";

/* Soft GStreamer READY state catalog id (GST_STATE_READY = 2). */
#define B9322_GST_STATE_READY  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9322_state_ready_id(void)
{
	return B9322_GST_STATE_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gst_state_ready_id_9322 - soft GStreamer READY state catalog id.
 *
 * Always returns 2. Soft continuum constant; does not set element
 * state or call GStreamer/libc. No parent wires.
 */
uint32_t
gj_gst_state_ready_id_9322(void)
{
	(void)NULL;
	return b9322_state_ready_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gst_state_ready_id_9322(void)
    __attribute__((alias("gj_gst_state_ready_id_9322")));
